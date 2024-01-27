/*!
 *  @author    Dubsky Tomas
 */
#include <bitset>
#include <iostream>

#include <ImGui/imgui_impl_sdl.h>
#include <ImGui/imgui_impl_vulkan.h>
#include <SDL2/SDL_vulkan.h>
#include <glm/common.hpp>
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.hpp>

#include <RealEngine/renderer/VulkanFixture.hpp>
#include <RealEngine/utility/Error.hpp>
#include <RealEngine/window/WindowSubsystems.hpp>

using enum vk::DebugUtilsMessageSeverityFlagBitsEXT;
using enum vk::DebugUtilsMessageTypeFlagBitsEXT;
using enum vk::SharingMode;
using enum vk::ImageUsageFlagBits;
using enum vk::CompositeAlphaFlagBitsKHR;

using enum vk::PresentModeKHR;
using enum vk::ImageViewType;
using enum vk::CommandBufferLevel;

namespace {

constexpr auto k_maxTimeout = std::numeric_limits<uint64_t>::max();

void checkSuccess(vk::Result res) {
    assert(res == vk::Result::eSuccess);
}

void checkSuccessImGui(VkResult res) {
    checkSuccess(vk::Result{res});
}

} // namespace

namespace re {

constexpr std::array k_deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

VulkanFixture::VulkanFixture(
    SDL_Window* sdlWindow, bool vSync, const VulkanInitInfo& initInfo
)
    : m_sdlWindow(sdlWindow)
    , m_vSync(vSync)
    , m_instance(createInstance())
#ifndef NDEBUG
    , m_debugUtilsMessenger(createDebugUtilsMessenger())
#endif // !NDEBUG
    , m_surface(createSurface())
    , m_physicalDevice(createPhysicalDevice())
    , m_device(createDevice(initInfo.deviceCreateInfoChain))
    , m_allocator(createAllocator())
    , m_graphicsQueue(createQueue(m_graphicsQueueFamilyIndex))
    , m_computeQueue(createQueue(m_computeQueueFamilyIndex))
    , m_presentationQueue(createQueue(m_presentationQueueFamilyIndex))
    , m_swapchain(createSwapchain())
    , m_swapchainImageViews(createSwapchainImageViews())
    , m_additionalBufferDescrs(
          {initInfo.additionalBuffers.begin(), initInfo.additionalBuffers.end()}
      )
    , m_additionalBuffers(createAdditionalBuffers())
    , m_renderPass(
          initInfo.mainRenderPass
              ? vk::raii::RenderPass{m_device, *initInfo.mainRenderPass}
              : createRenderPass()
      )
    , m_swapChainFramebuffers(createSwapchainFramebuffers())
    , m_commandPool(createCommandPool())
    , m_cmdBufs([&]() {
        assignImplementationReferences(); // Deliberate side effect, ref to device
                                          // and pool is required to construct a cmd buf
        return FrameDoubleBuffered{
            CommandBuffer{{.debugName = "re::VulkanFixture::cmdBufs[0]"}},
            CommandBuffer{{.debugName = "re::VulkanFixture::cmdBufs[1]"}}};
    }())
    , m_oneTimeSubmitCmdBuf({.debugName = "re::VulkanFixture::oneTimeSubmit"})
    , m_pipelineCache(createPipelineCache())
    , m_descriptorPool(createDescriptorPool())
    , m_imageAvailableSems(createSemaphores())
    , m_renderingFinishedSems(createSemaphores())
    , m_inFlightFences(createFences()) {
    // Implementations
    assignImplementationReferences();
    FrameDoubleBufferingState::setTotalIndex(m_frame++);

    // Initialize ImGui
    if (!ImGui_ImplSDL2_InitForVulkan(m_sdlWindow)) {
        throw std::runtime_error{"Could not initialize ImGui-SDL2 for Vulkan!"};
    }
    ImGui_ImplVulkan_InitInfo imGuiInitInfo{
        .Instance        = *m_instance,
        .PhysicalDevice  = *m_physicalDevice,
        .Device          = *m_device,
        .QueueFamily     = m_graphicsQueueFamilyIndex,
        .Queue           = *m_graphicsQueue,
        .PipelineCache   = *m_pipelineCache,
        .DescriptorPool  = *m_descriptorPool,
        .Subpass         = initInfo.imGuiSubpassIndex,
        .MinImageCount   = m_minImageCount,
        .ImageCount      = static_cast<uint32_t>(m_swapchainImageViews.size()),
        .MSAASamples     = VK_SAMPLE_COUNT_1_BIT,
        .Allocator       = nullptr,
        .CheckVkResultFn = &checkSuccessImGui};
    if (!ImGui_ImplVulkan_Init(&imGuiInitInfo, *m_renderPass)) {
        ImGui_ImplSDL2_Shutdown();
        throw std::runtime_error{"Could not initialize ImGui for Vulkan!"};
    }
}

VulkanFixture::~VulkanFixture() {
    m_device.waitIdle();
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
}

const CommandBuffer& VulkanFixture::prepareFrame(bool useImGui) {
    // Wait for the previous frame to finish
    checkSuccess(m_device.waitForFences(*m_inFlightFences.write(), true, k_maxTimeout)
    );

    m_deletionQueue.deleteNextGroup();
    m_deletionQueue.beginNewGroup();

    // Recreate swapchain if required
    if (m_recreteSwapchain) {
        recreateSwapchain();
        m_recreteSwapchain = false;
    }

    m_device.resetFences(*m_inFlightFences.write());

    // Acquire next image
    vk::AcquireNextImageInfoKHR acquireNextImageInfo{
        *m_swapchain, k_maxTimeout, *m_imageAvailableSems.write(), nullptr, 1u};
    auto [res, imageIndex] = m_device.acquireNextImage2KHR(acquireNextImageInfo);
    checkSuccess(res);
    m_imageIndex = imageIndex;

    // Rebuild fonts if new font were added
    if (!ImGui::GetIO().Fonts->IsBuilt()) {
        recreateImGuiFontTexture();
    }

    // Restart command buffer
    auto& cmdBuf = m_cmdBufs.write();
    cmdBuf->reset();
    cmdBuf->begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    // Set current commandbuffer
    VulkanObject::s_cmdBuf = &cmdBuf;

    // Begin ImGui frame
    if (useImGui) {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    return cmdBuf;
}

void VulkanFixture::mainRenderPassBegin(std::span<const vk::ClearValue> clearValues
) {
    auto& cmdBuf = m_cmdBufs.write();
    cmdBuf->beginRenderPass2(
        vk::RenderPassBeginInfo{
            *m_renderPass,
            *m_swapChainFramebuffers[m_imageIndex],
            vk::Rect2D{{}, m_swapchainExtent},
            clearValues},
        vk::SubpassBeginInfo{vk::SubpassContents::eInline}
    );

    // Set default viewport & scissor
    glm::vec2 extent{m_swapchainExtent.width, m_swapchainExtent.height};
    cmdBuf->setViewport(
        0u,
        vk::Viewport{
            0.0f,      // x
            extent.y,  // y
            extent.x,  // width
            -extent.y, // height
            0.0f,      // minDepth
            1.0f       // maxDepth
        }
    );
    cmdBuf->setScissor(
        0u,
        vk::Rect2D{
            {0, 0},                                             // x, y
            {m_swapchainExtent.width, m_swapchainExtent.height} // width, height
        }
    );
}

void VulkanFixture::mainRenderPassNextSubpass() {
    m_cmdBufs.write()->nextSubpass2(
        vk::SubpassBeginInfo{vk::SubpassContents::eInline}, vk::SubpassEndInfo{}
    );
}

void VulkanFixture::mainRenderPassDrawImGui() {
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *m_cmdBufs.write());
}

void VulkanFixture::mainRenderPassEnd() {
    auto& cmdBuf = m_cmdBufs.write();
    cmdBuf->endRenderPass2(vk::SubpassEndInfo{});
}

void VulkanFixture::finishFrame() {
    auto& cmdBuf = m_cmdBufs.write();
    cmdBuf->end();

    // Submit the command buffer
    vk::PipelineStageFlags waitDstStageMask =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::SubmitInfo submitInfo{
        *m_imageAvailableSems.write(), // Wait for image to be available
        waitDstStageMask,              // Wait just before writing output
        *cmdBuf,
        *m_renderingFinishedSems.write() // Signal that the rendering has
                                         // finished once done
    };
    m_graphicsQueue.submit(submitInfo, *m_inFlightFences.write());

    // Present new image
    vk::PresentInfoKHR presentInfo{
        *m_renderingFinishedSems.write(), // Wait for rendering to finish
        *m_swapchain,
        m_imageIndex};

    try {
        checkSuccess(m_presentationQueue.presentKHR(presentInfo));
    } catch (vk::OutOfDateKHRError&) { recreateSwapchain(); }

    FrameDoubleBufferingState::setTotalIndex(m_frame++);
}

void VulkanFixture::changePresentation(bool vSync) {
    m_vSync            = vSync;
    m_recreteSwapchain = true;
}

void VulkanFixture::prepareForDestructionOfRendererObjects() {
    m_device.waitIdle();
}

vk::raii::Instance VulkanFixture::createInstance() {
    // Prepare default layers and extensions
    std::vector<const char*> extensions = {
#ifndef NDEBUG
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif // !NDEBUG
    };
    std::vector<const char*> validationLayers = {
#ifndef NDEBUG
        "VK_LAYER_KHRONOS_validation", "VK_LAYER_KHRONOS_synchronization2"
#endif // !NDEBUG
    };

    // Add extensions required by SDL2
    unsigned int sdl2ExtensionCount;
    if (!SDL_Vulkan_GetInstanceExtensions(m_sdlWindow, &sdl2ExtensionCount, nullptr)) {
        throw std::runtime_error(
            "Could not get number of Vulkan extensions required for SDL2!"
        );
    }
    size_t defaultExtensionsCount = extensions.size();
    extensions.resize(defaultExtensionsCount + sdl2ExtensionCount);
    if (!SDL_Vulkan_GetInstanceExtensions(
            m_sdlWindow, &sdl2ExtensionCount, &extensions[defaultExtensionsCount]
        )) {
        throw std::runtime_error(
            "Could not get Vulkan extensions required for SDL2!"
        );
    }

    // Create Vulkan instance
    vk::ApplicationInfo applicationInfo(
        "RealEngine", 1, "RealEngine", k_version, vk::ApiVersion13
    );
    vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{
        {},
        eVerbose | eInfo | eWarning | eError,
        eGeneral | eValidation | ePerformance,
        &VulkanFixture::debugMessengerCallback};
    return vk::raii::Instance{
        m_context,
        vk::InstanceCreateInfo{
            {}, &applicationInfo, validationLayers, extensions, &debugMessengerCreateInfo}};
}

vk::raii::DebugUtilsMessengerEXT VulkanFixture::createDebugUtilsMessenger() {
    vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{
        {},
        eVerbose | eInfo | eWarning | eError,
        eGeneral | eValidation | ePerformance,
        &VulkanFixture::debugMessengerCallback};
    return vk::raii::DebugUtilsMessengerEXT{m_instance, debugMessengerCreateInfo};
}

vk::raii::SurfaceKHR VulkanFixture::createSurface() {
    VkSurfaceKHR surface;
    if (!SDL_Vulkan_CreateSurface(m_sdlWindow, *m_instance, &surface)) {
        throw std::runtime_error("SDL2 could not create Vulkan surface!");
    }
    return vk::raii::SurfaceKHR{m_instance, surface};
}

vk::raii::PhysicalDevice VulkanFixture::createPhysicalDevice() {
    for (const auto& physicalDevice : vk::raii::PhysicalDevices{m_instance}) {
        if (areExtensionsSupported(physicalDevice) &&
            isSwapchainSupported(physicalDevice) &&
            findQueueFamilyIndices(physicalDevice)) {
            auto props = physicalDevice.getProperties2();
            auto major = vk::versionMajor(props.properties.apiVersion);
            auto minor = vk::versionMinor(props.properties.apiVersion);
            auto patch = vk::versionPatch(props.properties.apiVersion);
            if (major == 1 && minor >= 3) {
                std::cout << "Vulkan:       " << major << '.' << minor << '.'
                          << patch << std::endl;
                return physicalDevice;
            }
        }
    }
    throw std::runtime_error("No physical device is suitable!");
}

vk::raii::Device VulkanFixture::createDevice(const void* deviceCreateInfoChain) {
    float                                  deviceQueuePriority = 1.0f;
    std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos;
    deviceQueueCreateInfos.emplace_back(
        vk::DeviceQueueCreateFlags{}, m_graphicsQueueFamilyIndex, 1, &deviceQueuePriority
    );
    if (m_graphicsQueueFamilyIndex != m_presentationQueueFamilyIndex) {
        deviceQueueCreateInfos.emplace_back(
            vk::DeviceQueueCreateFlags{}, m_presentationQueueFamilyIndex, 1, &deviceQueuePriority
        );
    }
    if (m_graphicsQueueFamilyIndex != m_computeQueueFamilyIndex) {
        deviceQueueCreateInfos.emplace_back(
            vk::DeviceQueueCreateFlags{}, m_computeQueueFamilyIndex, 1, &deviceQueuePriority
        );
    }
    auto createInfo =
        vk::DeviceCreateInfo{{}, deviceQueueCreateInfos, {}, k_deviceExtensions};

    auto defaultChain = vk::StructureChain{
        vk::PhysicalDeviceFeatures2{
            vk::PhysicalDeviceFeatures{}.setTessellationShader(true)},
        vk::PhysicalDeviceVulkan12Features{}
            .setShaderSampledImageArrayNonUniformIndexing(true)
            .setDescriptorBindingUpdateUnusedWhilePending(true)
            .setDescriptorBindingPartiallyBound(true)
            .setTimelineSemaphore(true),
        vk::PhysicalDeviceVulkan13Features{}.setSynchronization2(true)};

    createInfo.pNext = deviceCreateInfoChain
                           ? deviceCreateInfoChain
                           : reinterpret_cast<const void*>(&defaultChain.get<>());
    return vk::raii::Device{m_physicalDevice, createInfo};
}

vma::Allocator VulkanFixture::createAllocator() {
    return vma::createAllocator(vma::AllocatorCreateInfo{
        vma::AllocatorCreateFlagBits::eExternallySynchronized,
        *m_physicalDevice,
        *m_device,
        0,       // Use default large heap block size (256 MB as of writing)
        nullptr, // Use default CPU memory allocation callbacks
        nullptr, // Do not receive informative callbacks from VMA
        nullptr, // No heap size limits
        nullptr, // Let VMA load Vulkan functions on its own
        *m_instance,
        vk::ApiVersion13,
        nullptr, // No external memory handles
    });
}

vk::raii::Queue VulkanFixture::createQueue(uint32_t familyIndex) {
    return vk::raii::Queue{m_device, familyIndex, 0u};
}

vk::raii::SwapchainKHR VulkanFixture::createSwapchain() {
    auto caps = m_physicalDevice.getSurfaceCapabilitiesKHR(*m_surface);
    // Minimum image count
    m_minImageCount = glm::clamp(
        caps.minImageCount + 1,
        caps.minImageCount,
        caps.maxImageCount ? caps.maxImageCount : 8u
    );

    // Extent
    if (caps.currentExtent == vk::Extent2D{0xFFFFFFFF, 0xFFFFFFFF}) {
        m_swapchainExtent = caps.currentExtent;
    } else {
        glm::ivec2 windowPx;
        SDL_Vulkan_GetDrawableSize(m_sdlWindow, &windowPx.x, &windowPx.y);
        m_swapchainExtent.width = std::clamp(
            static_cast<uint32_t>(windowPx.x),
            caps.minImageExtent.width,
            caps.maxImageExtent.width
        );
        m_swapchainExtent.height = std::clamp(
            static_cast<uint32_t>(windowPx.y),
            caps.minImageExtent.height,
            caps.maxImageExtent.height
        );
    }

    // Sharing mode
    bool oneQueueFamily = m_graphicsQueueFamilyIndex ==
                          m_presentationQueueFamilyIndex;
    auto       sharingMode        = oneQueueFamily ? eExclusive : eConcurrent;
    std::array queueFamilyIndices = {
        m_graphicsQueueFamilyIndex, m_presentationQueueFamilyIndex};
    vk::SwapchainCreateInfoKHR createInfo{
        {},
        *m_surface,
        m_minImageCount,
        k_surfaceFormat.format,
        k_surfaceFormat.colorSpace,
        m_swapchainExtent,
        1u,
        eColorAttachment,
        sharingMode,
        oneQueueFamily ? vk::ArrayProxyNoTemporaries<const uint32_t>{}
                       : queueFamilyIndices,
        caps.currentTransform,
        eOpaque,
        m_vSync ? eMailbox : eImmediate,
        true};
    return vk::raii::SwapchainKHR{m_device, createInfo};
}

std::vector<vk::raii::ImageView> VulkanFixture::createSwapchainImageViews() {
    auto                             images = m_swapchain.getImages();
    std::vector<vk::raii::ImageView> imageViews;
    imageViews.reserve(images.size());
    for (const auto& image : images) {
        imageViews.emplace_back(
            m_device,
            vk::ImageViewCreateInfo{
                {},
                image,
                e2D,
                k_surfaceFormat.format,
                {},
                vk::ImageSubresourceRange{
                    vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u}}
        );
    }
    return imageViews;
}

std::vector<Texture> VulkanFixture::createAdditionalBuffers() {
    // Ugly hack to be able to use re::Texture already
    VulkanObject::s_allocator = &m_allocator;
    VulkanObject::s_device    = &(*m_device);
    std::vector<Texture> buffers;
    buffers.reserve(m_additionalBufferDescrs.size());
    for (const auto& descr : m_additionalBufferDescrs) {
        buffers.emplace_back(TextureCreateInfo{
            .allocFlags = vma::AllocationCreateFlagBits::eDedicatedMemory,
            .format     = descr.format,
            .extent = glm::uvec3{m_swapchainExtent.width, m_swapchainExtent.height, 1},
            .usage         = descr.usage,
            .initialLayout = vk::ImageLayout::eUndefined,
            .aspects       = descr.aspects,
            .hasSampler    = false});
    }
    return buffers;
}

vk::raii::RenderPass VulkanFixture::createRenderPass() {
    vk::AttachmentDescription2 attachmentDescription{
        {},
        k_surfaceFormat.format,
        vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eClear,     // Color
        vk::AttachmentStoreOp::eStore,    // Color
        vk::AttachmentLoadOp::eDontCare,  // Stencil
        vk::AttachmentStoreOp::eDontCare, // Stencil
        vk::ImageLayout::eUndefined,      // Initial
        vk::ImageLayout::ePresentSrcKHR   // Final
    };
    vk::AttachmentReference2 attachmentRef{0, vk::ImageLayout::eColorAttachmentOptimal};
    vk::SubpassDescription2 subpassDescription{
        {},
        vk::PipelineBindPoint::eGraphics,
        0u,
        {},           // Input attachments
        attachmentRef // Color attachments
    };
    vk::SubpassDependency2 subpassDependency{
        vk::SubpassExternal,                                 // Src subpass
        0u,                                                  // Dst subpass
        {vk::PipelineStageFlagBits::eColorAttachmentOutput}, // Src stage mask
        {vk::PipelineStageFlagBits::eColorAttachmentOutput}, // Dst stage mask
        vk::AccessFlags{},                                   // Src access mask
        {vk::AccessFlagBits::eColorAttachmentWrite}          // Dst access mask
    };
    vk::RenderPassCreateInfo2 createInfo{
        {}, attachmentDescription, subpassDescription, subpassDependency};
    return vk::raii::RenderPass{m_device, createInfo};
}

std::vector<vk::raii::Framebuffer> VulkanFixture::createSwapchainFramebuffers() {
    std::vector<vk::ImageView> views;
    views.resize(1 + m_additionalBuffers.size());
    vk::FramebufferCreateInfo createInfo{
        {}, *m_renderPass, views, m_swapchainExtent.width, m_swapchainExtent.height, 1};

    // Additional buffer are shared among the framebuffers
    for (size_t addtBuf = 0; addtBuf < m_additionalBuffers.size(); ++addtBuf) {
        views[addtBuf + 1] = m_additionalBuffers[addtBuf].imageView();
    }

    // Swapchain images are different
    std::vector<vk::raii::Framebuffer> framebuffers;
    framebuffers.reserve(m_swapchainImageViews.size());
    for (size_t i = 0; i < m_swapchainImageViews.size(); i++) {
        views[0] = *m_swapchainImageViews[i];
        framebuffers.emplace_back(m_device, createInfo);
    }
    return framebuffers;
}

vk::raii::CommandPool VulkanFixture::createCommandPool() {
    vk::CommandPoolCreateInfo createInfo{
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        m_graphicsQueueFamilyIndex};
    return vk::raii::CommandPool{m_device, createInfo};
}

FrameDoubleBuffered<vk::raii::Semaphore> VulkanFixture::createSemaphores() {
    vk::SemaphoreCreateInfo createInfo{};
    return {
        vk::raii::Semaphore{m_device, createInfo},
        vk::raii::Semaphore{m_device, createInfo}};
}

FrameDoubleBuffered<vk::raii::Fence> VulkanFixture::createFences() {
    vk::FenceCreateInfo createInfo{vk::FenceCreateFlagBits::eSignaled};
    return {
        vk::raii::Fence{m_device, createInfo},
        vk::raii::Fence{m_device, createInfo}};
}

vk::raii::PipelineCache VulkanFixture::createPipelineCache() {
    return vk::raii::PipelineCache{m_device, vk::PipelineCacheCreateInfo{}};
}

vk::raii::DescriptorPool VulkanFixture::createDescriptorPool() {
    using enum vk::DescriptorType;
    constexpr std::array poolSizes = std::to_array<vk::DescriptorPoolSize>({
        {eSampler, 128},
        {eCombinedImageSampler, 128},
        {eSampledImage, 128},
        {eStorageImage, 128},
        {eUniformTexelBuffer, 128},
        {eStorageTexelBuffer, 128},
        {eUniformBuffer, 128},
        {eStorageBuffer, 128},
        {eUniformBufferDynamic, 128},
        {eStorageBufferDynamic, 128},
        {eInputAttachment, 128},
    });
    vk::DescriptorPoolCreateInfo createInfo{
        {},
        // 8 is 'just enough' - deserves a better solution
        static_cast<uint32_t>(m_swapchainImageViews.size()) * 8u,
        poolSizes};
    return vk::raii::DescriptorPool{m_device, createInfo};
}

VkBool32 VulkanFixture::debugMessengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      sev,
    VkDebugUtilsMessageTypeFlagsEXT             type,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void*                                       userData
) {
    // If not a loader message...
    if (!callbackData->pMessageIdName ||
        std::strcmp(callbackData->pMessageIdName, "Loader Message") != 0) {
        // Show nested debug labels
        std::string str{"##"};
        for (int i = 0; i < callbackData->cmdBufLabelCount; i++) {
            str += callbackData->pCmdBufLabels[i].pLabelName;
            if (i < callbackData->cmdBufLabelCount - 1) {
                str += "->";
            }
        }
        str += "\n  ";

        // Do not show the silly beginning of the message
        const char* msg   = callbackData->pMessage;
        int         skips = 0;
        while (*msg != '\0') {
            bool pipe = *msg == '|';
            msg++;
            if (pipe) {
                skips++;
                if (skips == 2)
                    break;
            }
        }

        // Report the message
        switch (static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(sev)) {
        case eVerbose:
        case eInfo:
        case eWarning: log(str + msg); break;
        case eError:
        default: error(str + msg); break;
        }
    }
    return false;
}

bool VulkanFixture::areExtensionsSupported(const vk::raii::PhysicalDevice& physicalDevice
) {
    std::bitset<k_deviceExtensions.size()> supported{};
    for (const auto& ext : physicalDevice.enumerateDeviceExtensionProperties()) {
        for (size_t i = 0; i < k_deviceExtensions.size(); ++i) {
            if (std::strcmp(ext.extensionName.data(), k_deviceExtensions[i]) == 0) {
                supported[i] = true;
            }
        }
    }
    return supported.all();
}

bool VulkanFixture::isSwapchainSupported(const vk::raii::PhysicalDevice& physicalDevice
) {
    bool formatSupported = false;
    for (const auto& format : physicalDevice.getSurfaceFormatsKHR(*m_surface)) {
        if (format == k_surfaceFormat) {
            formatSupported = true;
            break;
        }
    }
    bool mailboxSupported = false;
    for (const auto& presentMode :
         physicalDevice.getSurfacePresentModesKHR(*m_surface)) {
        if (presentMode == vk::PresentModeKHR::eMailbox) {
            mailboxSupported = true;
            break;
        }
    }
    return formatSupported && mailboxSupported;
}

bool VulkanFixture::findQueueFamilyIndices(const vk::raii::PhysicalDevice& physicalDevice
) {
    bool                                   graphicsQueueFound = false;
    bool                                   computeQueueFound  = false;
    bool                                   presentQueueFound  = false;
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
        physicalDevice.getQueueFamilyProperties();
    for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size());
         i++) { // Iterate over all queue families
        if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            m_graphicsQueueFamilyIndex = i;
            graphicsQueueFound         = true;
        }
        if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eCompute) {
            m_computeQueueFamilyIndex = i;
            computeQueueFound         = true;
        }
        if (physicalDevice.getSurfaceSupportKHR(i, *m_surface)) {
            m_presentationQueueFamilyIndex = i;
            presentQueueFound              = true;
        }
        if (graphicsQueueFound && computeQueueFound && presentQueueFound) {
            return true;
        }
    }
    return false;
}

void VulkanFixture::recreateSwapchain() {
    m_device.waitIdle();
    // Destroy all swapchain dependent objects
    m_swapChainFramebuffers.~vector();
    m_additionalBuffers.~vector();
    m_swapchainImageViews.~vector();
    m_swapchain.~SwapchainKHR();

    // Recreate them with the new size
    new (&m_swapchain) decltype(m_swapchain){createSwapchain()};
    new (&m_swapchainImageViews) decltype(m_swapchainImageViews
    ){createSwapchainImageViews()};
    new (&m_additionalBuffers) decltype(m_additionalBuffers
    ){createAdditionalBuffers()};
    new (&m_swapChainFramebuffers) decltype(m_swapChainFramebuffers
    ){createSwapchainFramebuffers()};
}

void VulkanFixture::recreateImGuiFontTexture() {
    vk::raii::Fence uploadFence{m_device, vk::FenceCreateInfo{}};
    m_cmdBufs.write()->begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    ImGui_ImplVulkan_CreateFontsTexture(*m_cmdBufs.write());
    m_cmdBufs.write()->end();
    m_graphicsQueue.submit(vk::SubmitInfo{{}, {}, *m_cmdBufs.write()}, *uploadFence);
    checkSuccess(m_device.waitForFences(*uploadFence, true, k_maxTimeout));
}

void VulkanFixture::assignImplementationReferences() {
    VulkanObject::s_physicalDevice        = &(*m_physicalDevice);
    VulkanObject::s_device                = &(*m_device);
    VulkanObject::s_allocator             = &m_allocator;
    VulkanObject::s_graphicsQueue         = &(*m_graphicsQueue);
    VulkanObject::s_computeQueue          = &(*m_computeQueue);
    VulkanObject::s_commandPool           = &(*m_commandPool);
    VulkanObject::s_descriptorPool        = &(*m_descriptorPool);
    VulkanObject::s_pipelineCache         = &(*m_pipelineCache);
    VulkanObject::s_renderPass            = &(*m_renderPass);
    VulkanObject::s_oneTimeSubmitCmdBuf   = &m_oneTimeSubmitCmdBuf;
    VulkanObject::s_dispatchLoaderDynamic = &(m_dispatchLoaderDynamic);
    VulkanObject::s_deletionQueue         = &m_deletionQueue;
}

} // namespace re
