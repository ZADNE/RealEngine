/*!
 *  @author    Dubsky Tomas
 */
#include <bitset>
#include <iostream>

#include <ImGui/imgui_impl_sdl.h>
#include <ImGui/imgui_impl_vulkan.h>
#include <SDL2/SDL_vulkan.h>
#include <glm/common.hpp>

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

namespace RE {

constexpr std::array k_deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

constexpr vk::SurfaceFormatKHR k_surfaceFormat{
    vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};

VulkanFixture::VulkanFixture(SDL_Window* sdlWindow, bool vSync)
    : m_sdlWindow(sdlWindow)
    , m_vSync(vSync)
    , m_instance(createInstance())
#ifndef NDEBUG
    , m_debugUtilsMessenger(createDebugUtilsMessenger())
#endif // !NDEBUG
    , m_surface(createSurface())
    , m_physicalDevice(createPhysicalDevice())
    , m_device(createDevice())
    , m_graphicsQueue(createQueue(m_graphicsQueueFamilyIndex))
    , m_computeQueue(createQueue(m_computeQueueFamilyIndex))
    , m_presentationQueue(createQueue(m_presentationQueueFamilyIndex))
    , m_swapchain(createSwapchain())
    , m_swapchainImageViews(createSwapchainImageViews())
    , m_renderPass(createRenderPass())
    , m_swapChainFramebuffers(createSwapchainFramebuffers())
    , m_commandPool(createCommandPool())
    , m_commandBuffers(createCommandBuffers())
    , m_oneTimeSubmitCommandBuffer(std::move(vk::raii::CommandBuffers{
          m_device,
          {*m_commandPool, ePrimary, 1u}}.back()))
    , m_pipelineCache(createPipelineCache())
    , m_descriptorPool(createDescriptorPool())
    , m_imageAvailableSems(createSemaphores())
    , m_renderingFinishedSems(createSemaphores())
    , m_inFlightFences(createFences()) {
    // Implementations
    assignImplementationReferences();
    setFramesInFlight(2);
    setFrame(m_frame++);
    // Initialize ImGui
    if (!ImGui_ImplSDL2_InitForVulkan(m_sdlWindow)) {
        throw std::runtime_error{"Could not initialize ImGui-SDL2 for Vulkan!"};
    }
    ImGui_ImplVulkan_InitInfo initInfo{
        .Instance        = *m_instance,
        .PhysicalDevice  = *m_physicalDevice,
        .Device          = *m_device,
        .QueueFamily     = m_graphicsQueueFamilyIndex,
        .Queue           = *m_graphicsQueue,
        .PipelineCache   = *m_pipelineCache,
        .DescriptorPool  = *m_descriptorPool,
        .Subpass         = 0u,
        .MinImageCount   = m_minImageCount,
        .ImageCount      = static_cast<uint32_t>(m_swapchainImageViews.size()),
        .MSAASamples     = VK_SAMPLE_COUNT_1_BIT,
        .Allocator       = nullptr,
        .CheckVkResultFn = &checkSuccessImGui};
    if (!ImGui_ImplVulkan_Init(&initInfo, *m_renderPass)) {
        ImGui_ImplSDL2_Shutdown();
        throw std::runtime_error{"Could not initialize ImGui for Vulkan!"};
    }
}

VulkanFixture::~VulkanFixture() {
    m_device.waitIdle();
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    clearImplementationReferences();
}

const vk::CommandBuffer&
VulkanFixture::prepareFrame(const glm::vec4& clearColor, bool useImGui) {
    // Wait for the previous frame to finish
    checkSuccess(
        m_device.waitForFences(*current(m_inFlightFences), true, k_maxTimeout)
    );

    m_deletionQueue.deleteNextGroup();
    m_deletionQueue.beginNewGroup();

    // Recreate swapchain if required
    if (m_recreteSwapchain) {
        recreateSwapchain();
        m_recreteSwapchain = false;
    }

    m_device.resetFences(*current(m_inFlightFences));

    // Acquire next image
    vk::AcquireNextImageInfoKHR acquireNextImageInfo{
        *m_swapchain,
        k_maxTimeout,
        *current(m_imageAvailableSems),
        nullptr,
        1u};
    auto [res, imageIndex] =
        m_device.acquireNextImage2KHR(acquireNextImageInfo);
    checkSuccess(res);
    m_imageIndex = imageIndex;

    // Rebuild fonts if new font were added
    if (!ImGui::GetIO().Fonts->IsBuilt()) {
        recreateImGuiFontTexture();
    }

    // Restart command buffer
    auto& commandBuffer = current(m_commandBuffers);
    commandBuffer.reset();
    commandBuffer.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    // Set current commandbuffer
    VulkanObject::s_commandBuffer = &(*commandBuffer);

    // Begin renderpass
    const auto* clearColorPtr =
        reinterpret_cast<const std::array<float, 4u>*>(&clearColor);
    vk::ClearValue          clearValue{vk::ClearColorValue{*clearColorPtr}};
    vk::RenderPassBeginInfo renderPassBeginInfo{
        *m_renderPass,
        *m_swapChainFramebuffers[m_imageIndex],
        vk::Rect2D{{}, m_swapchainExtent},
        clearValue};
    commandBuffer.beginRenderPass(
        renderPassBeginInfo, vk::SubpassContents::eInline
    );

    // Begin ImGui frame
    if (useImGui) {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    // Set default viewport & scissor
    glm::vec2 extent{m_swapchainExtent.width, m_swapchainExtent.height};
    commandBuffer.setViewport(
        0u,
        vk::Viewport{
            0.0f,
            extent.y, // X, Y
            extent.x,
            -extent.y, // Width, height
            0.0f,
            1.0f // MinDepth, MaxDepth
        }
    );
    commandBuffer.setScissor(
        0u,
        vk::Rect2D{
            {0, 0},                                             // X, Y
            {m_swapchainExtent.width, m_swapchainExtent.height} // Width, height
        }
    );

    return (*commandBuffer);
}

void VulkanFixture::finishFrame(bool useImGui) {
    auto& commandBuffer = current(m_commandBuffers);
    if (useImGui) {
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *commandBuffer);
    }

    // Submit the command buffer
    commandBuffer.endRenderPass();
    commandBuffer.end();
    vk::PipelineStageFlags waitDstStageMask =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::SubmitInfo submitInfo{
        *current(m_imageAvailableSems), // Wait for image to be available
        waitDstStageMask,               // Wait just before writing output
        *commandBuffer,
        *current(m_renderingFinishedSems) // Signal that the rendering has
                                          // finished once done
    };
    m_graphicsQueue.submit(submitInfo, *current(m_inFlightFences));

    // Present new image
    vk::PresentInfoKHR presentInfo{
        *current(m_renderingFinishedSems), // Wait for rendering to finish
        *m_swapchain,
        m_imageIndex};

    try {
        checkSuccess(m_presentationQueue.presentKHR(presentInfo));
    } catch (vk::OutOfDateKHRError&) { recreateSwapchain(); }

    setFrame(m_frame++);
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
    if (!SDL_Vulkan_GetInstanceExtensions(
            m_sdlWindow, &sdl2ExtensionCount, nullptr
        )) {
        throw std::runtime_error(
            "Could not get number of Vulkan extensions required for SDL2!"
        );
    }
    size_t defaultExtensionsCount = extensions.size();
    extensions.resize(defaultExtensionsCount + sdl2ExtensionCount);
    if (!SDL_Vulkan_GetInstanceExtensions(
            m_sdlWindow,
            &sdl2ExtensionCount,
            &extensions[defaultExtensionsCount]
        )) {
        throw std::runtime_error(
            "Could not get Vulkan extensions required for SDL2!"
        );
    }

    // Create Vulkan instance
    vk::ApplicationInfo applicationInfo(
        "RealEngine", 1, "RealEngine", k_version, VK_API_VERSION_1_3
    );
    vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{
        {},
        /*eVerbose | eInfo |*/ eWarning | eError,
        eGeneral | eValidation | ePerformance,
        &VulkanFixture::debugMessengerCallback};
    return vk::raii::Instance{
        m_context,
        vk::InstanceCreateInfo{
            {},
            &applicationInfo,
            validationLayers,
            extensions,
            &debugMessengerCreateInfo}};
}

vk::raii::DebugUtilsMessengerEXT VulkanFixture::createDebugUtilsMessenger() {
    vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{
        {},
        /*eVerbose | eInfo |*/ eWarning | eError,
        eGeneral | eValidation | ePerformance,
        &VulkanFixture::debugMessengerCallback};
    return vk::raii::DebugUtilsMessengerEXT{
        m_instance, debugMessengerCreateInfo};
}

vk::raii::SurfaceKHR VulkanFixture::createSurface() {
    VkSurfaceKHR surface;
    if (!SDL_Vulkan_CreateSurface(m_sdlWindow, *m_instance, &surface)) {
        throw std::runtime_error("SDL2 could not create Vulkan surface!");
    }
    return vk::raii::SurfaceKHR{m_instance, surface};
}

vk::raii::PhysicalDevice VulkanFixture::createPhysicalDevice() {
    for (const auto& physicalDevice :
         vk::raii::PhysicalDevices{m_instance}) { // Iterate over all physical
                                                  // device
        if (areExtensionsSupported(physicalDevice) &&
            isSwapchainSupported(physicalDevice) &&
            findQueueFamilyIndices(physicalDevice)) {
            auto props = physicalDevice.getProperties2();
            auto major = VK_API_VERSION_MAJOR(props.properties.apiVersion);
            auto minor = VK_API_VERSION_MINOR(props.properties.apiVersion);
            auto patch = VK_API_VERSION_PATCH(props.properties.apiVersion);
            if (major == 1 && minor >= 3) {
                std::cout << "Vulkan:       " << major << '.' << minor << '.'
                          << patch << std::endl;
                return physicalDevice;
            }
        }
    }
    throw std::runtime_error("No physical device is suitable!");
}

vk::raii::Device VulkanFixture::createDevice() {
    float                                  deviceQueuePriority = 1.0f;
    std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos;
    deviceQueueCreateInfos.emplace_back(
        vk::DeviceQueueCreateFlags{},
        m_graphicsQueueFamilyIndex,
        1,
        &deviceQueuePriority
    );
    if (m_graphicsQueueFamilyIndex != m_presentationQueueFamilyIndex) {
        deviceQueueCreateInfos.emplace_back(
            vk::DeviceQueueCreateFlags{},
            m_presentationQueueFamilyIndex,
            1,
            &deviceQueuePriority
        );
    }
    if (m_graphicsQueueFamilyIndex != m_computeQueueFamilyIndex) {
        deviceQueueCreateInfos.emplace_back(
            vk::DeviceQueueCreateFlags{},
            m_computeQueueFamilyIndex,
            1,
            &deviceQueuePriority
        );
    }
    vk::PhysicalDeviceFeatures baseFeatures{};
    baseFeatures.setTessellationShader(true);
    baseFeatures.setWideLines(true);
    auto createInfo = vk::StructureChain{
        vk::DeviceCreateInfo{
            {}, deviceQueueCreateInfos, {}, k_deviceExtensions, &baseFeatures},
        vk::PhysicalDeviceUniformBufferStandardLayoutFeatures{true},
        vk::PhysicalDeviceDescriptorIndexingFeatures{},
        vk::PhysicalDeviceSynchronization2Features{true},
        vk::PhysicalDeviceTimelineSemaphoreFeatures{true}};
    auto& descIndexing =
        createInfo.get<vk::PhysicalDeviceDescriptorIndexingFeatures>();
    descIndexing.setShaderSampledImageArrayNonUniformIndexing(true);
    descIndexing.setDescriptorBindingUpdateUnusedWhilePending(true);
    descIndexing.setDescriptorBindingPartiallyBound(true);
    return vk::raii::Device{m_physicalDevice, createInfo.get<>()};
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
        caps.maxImageCount ? caps.maxImageCount : 8
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

vk::raii::RenderPass VulkanFixture::createRenderPass() {
    vk::AttachmentDescription attachmentDescription{
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
    vk::AttachmentReference attachmentRef{
        0, vk::ImageLayout::eColorAttachmentOptimal};
    vk::SubpassDescription subpassDescription{
        {},
        vk::PipelineBindPoint::eGraphics,
        {},           // Input attachments
        attachmentRef // Color attachments
    };
    vk::SubpassDependency subpassDependency{
        VK_SUBPASS_EXTERNAL,                                 // Src subpass
        0u,                                                  // Dst subpass
        {vk::PipelineStageFlagBits::eColorAttachmentOutput}, // Src stage
                                                             // mask
        {vk::PipelineStageFlagBits::eColorAttachmentOutput}, // Dst stage
                                                             // mask
        vk::AccessFlags{},                                   // Src access mask
        {vk::AccessFlagBits::eColorAttachmentWrite}          // Dst access mask
    };
    vk::RenderPassCreateInfo createInfo{
        {}, attachmentDescription, subpassDescription, subpassDependency};
    return vk::raii::RenderPass{m_device, createInfo};
}

std::vector<vk::raii::Framebuffer>
VulkanFixture::createSwapchainFramebuffers() {
    vk::FramebufferCreateInfo createInfo{
        {},
        *m_renderPass,
        {},
        m_swapchainExtent.width,
        m_swapchainExtent.height,
        1};
    std::vector<vk::raii::Framebuffer> vec;
    vec.reserve(m_swapchainImageViews.size());
    for (size_t i = 0; i < m_swapchainImageViews.size(); i++) {
        createInfo.setAttachments(*m_swapchainImageViews[i]);
        vec.emplace_back(m_device, createInfo);
    }
    return vec;
}

vk::raii::CommandPool VulkanFixture::createCommandPool() {
    vk::CommandPoolCreateInfo createInfo{
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        m_graphicsQueueFamilyIndex};
    return vk::raii::CommandPool{m_device, createInfo};
}

PerFrameInFlight<vk::raii::CommandBuffer>
VulkanFixture::createCommandBuffers() {
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo{
        *m_commandPool, ePrimary, k_maxFramesInFlight};
    vk::raii::CommandBuffers buffers{m_device, commandBufferAllocateInfo};
    assert(buffers.size() == k_maxFramesInFlight);
    return PerFrameInFlight<vk::raii::CommandBuffer>{
        std::move(buffers[0]), std::move(buffers[1])};
}

PerFrameInFlight<vk::raii::Semaphore> VulkanFixture::createSemaphores() {
    vk::SemaphoreCreateInfo createInfo{};
    return {
        vk::raii::Semaphore{m_device, createInfo},
        vk::raii::Semaphore{m_device, createInfo}};
}

PerFrameInFlight<vk::raii::Fence> VulkanFixture::createFences() {
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
        static_cast<uint32_t>(m_swapchainImageViews.size()) * 8u, //'8 is
                                                                  // just
                                                                  // enough'
                                                                  // - needs
                                                                  // more
                                                                  // work
        poolSizes};
    return vk::raii::DescriptorPool{m_device, createInfo};
}

VkBool32 VulkanFixture::debugMessengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      sev,
    VkDebugUtilsMessageTypeFlagsEXT             type,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void*                                       userData
) {
    if (sev != VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT &&
        std::strcmp(callbackData->pMessageIdName, "Loader Message") != 0) {
        error(callbackData->pMessage);
    }
    return false;
}

bool VulkanFixture::areExtensionsSupported(
    const vk::raii::PhysicalDevice& physicalDevice
) {
    std::bitset<k_deviceExtensions.size()> supported{};
    for (const auto& extensionProperties :
         physicalDevice.enumerateDeviceExtensionProperties()) {
        for (size_t i = 0; i < k_deviceExtensions.size(); ++i) {
            if (std::strcmp(
                    extensionProperties.extensionName.data(),
                    k_deviceExtensions[i]
                ) == 0) {
                supported[i] = true;
            }
        }
    }
    return supported.all();
}

bool VulkanFixture::isSwapchainSupported(
    const vk::raii::PhysicalDevice& physicalDevice
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

bool VulkanFixture::findQueueFamilyIndices(
    const vk::raii::PhysicalDevice& physicalDevice
) {
    bool                                   graphicsQueueFound = false;
    bool                                   computeQueueFound  = false;
    bool                                   presentQueueFound  = false;
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
        physicalDevice.getQueueFamilyProperties();
    for (uint32_t i = 0;
         i < static_cast<uint32_t>(queueFamilyProperties.size());
         i++) { // Iterate over all queue families
        if (queueFamilyProperties[i].queueFlags &
            vk::QueueFlagBits::eGraphics) {
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
    m_swapChainFramebuffers.~vector();
    m_swapchainImageViews.~vector();
    m_swapchain.~SwapchainKHR();
    new (&m_swapchain) decltype(m_swapchain){createSwapchain()};
    new (&m_swapchainImageViews) decltype(m_swapchainImageViews
    ){createSwapchainImageViews()};
    new (&m_swapChainFramebuffers) decltype(m_swapChainFramebuffers
    ){createSwapchainFramebuffers()};
}

void VulkanFixture::recreateImGuiFontTexture() {
    vk::raii::Fence uploadFence{m_device, vk::FenceCreateInfo{}};
    current(m_commandBuffers)
        .begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    ImGui_ImplVulkan_CreateFontsTexture(*current(m_commandBuffers));
    current(m_commandBuffers).end();
    m_graphicsQueue.submit(
        vk::SubmitInfo{{}, {}, *current(m_commandBuffers)}, *uploadFence
    );
    checkSuccess(m_device.waitForFences(*uploadFence, true, k_maxTimeout));
}

void VulkanFixture::assignImplementationReferences() {
    VulkanObject::s_physicalDevice = &(*m_physicalDevice);
    VulkanObject::s_device         = &(*m_device);
    VulkanObject::s_graphicsQueue  = &(*m_graphicsQueue);
    VulkanObject::s_computeQueue   = &(*m_computeQueue);
    VulkanObject::s_commandPool    = &(*m_commandPool);
    VulkanObject::s_descriptorPool = &(*m_descriptorPool);
    VulkanObject::s_pipelineCache  = &(*m_pipelineCache);
    VulkanObject::s_renderPass     = &(*m_renderPass);
    VulkanObject::s_oneTimeSubmitCommandBuffer =
        &(*m_oneTimeSubmitCommandBuffer);
    VulkanObject::s_deletionQueue = &m_deletionQueue;
}

void VulkanFixture::clearImplementationReferences() {
    VulkanObject::s_physicalDevice             = nullptr;
    VulkanObject::s_device                     = nullptr;
    VulkanObject::s_graphicsQueue              = nullptr;
    VulkanObject::s_computeQueue               = nullptr;
    VulkanObject::s_commandPool                = nullptr;
    VulkanObject::s_descriptorPool             = nullptr;
    VulkanObject::s_pipelineCache              = nullptr;
    VulkanObject::s_renderPass                 = nullptr;
    VulkanObject::s_oneTimeSubmitCommandBuffer = nullptr;
    VulkanObject::s_deletionQueue              = nullptr;
}

} // namespace RE