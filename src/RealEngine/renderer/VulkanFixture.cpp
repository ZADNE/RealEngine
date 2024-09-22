/*!
 *  @author    Dubsky Tomas
 */
#include <iostream>

#include <ImGui/imgui_impl_sdl2.h>
#include <ImGui/imgui_impl_vulkan.h>
#include <SDL_vulkan.h>
#include <glm/common.hpp>
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.hpp>

#include <RealEngine/renderer/DebugMessageHandler.hpp>
#include <RealEngine/renderer/PhysDeviceSuitability.hpp>
#include <RealEngine/renderer/VulkanFixture.hpp>
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

const void* defaultDeviceCreateInfoChain() {
    static auto s_default = vk::StructureChain{
        vk::PhysicalDeviceFeatures2{
            vk::PhysicalDeviceFeatures{}.setTessellationShader(true)
        },
        vk::PhysicalDeviceVulkan12Features{}
            .setShaderSampledImageArrayNonUniformIndexing(true)
            .setDescriptorBindingUpdateUnusedWhilePending(true)
            .setDescriptorBindingPartiallyBound(true)
            .setTimelineSemaphore(true),
        vk::PhysicalDeviceVulkan13Features{}.setSynchronization2(true)
    };
    return &s_default.get<>();
}

VulkanFixture::VulkanFixture(
    SDL_Window* sdlWindow, bool vSync, std::string_view preferredDevice,
    const VulkanInitInfo& initInfo
)
    : m_sdlWindow(sdlWindow)
    , m_instance(createInstance())
#ifndef NDEBUG
    , m_debugUtilsMessenger(createDebugUtilsMessenger())
#endif // !NDEBUG
    , m_surface(createSurface())
    , m_physicalDevice(
          createPhysicalDevice(preferredDevice, initInfo.deviceCreateInfoChain)
      )
    , m_presentMode(selectClosestPresentMode(vSync))
    , m_device(createDevice(initInfo.deviceCreateInfoChain))
    , m_allocator(createAllocator())
    , m_graphicsCompQueue(getQueue(m_graphicsCompQueueFamIndex))
    , m_presentationQueue(getQueue(m_presentationQueueFamIndex))
    , m_swapchain(createSwapchain())
    , m_swapchainImageViews(createSwapchainImageViews())
    , m_additionalBufferDescrs(
          {initInfo.additionalBuffers.begin(), initInfo.additionalBuffers.end()}
      )
    , m_additionalBuffers(createAdditionalBuffers())
    , m_commandPool(createCommandPool())
    , m_cbs([&]() {
        assignImplementationReferences(); // Deliberate side effect, ref to device
                                          // and pool is required to construct a cmd buf
        return FrameDoubleBuffered<CommandBuffer>{
            CommandBuffer{{.debugName = "re::VulkanFixture::cbs[0]"}},
            CommandBuffer{{.debugName = "re::VulkanFixture::cbs[1]"}}
        };
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

    VulkanObjectBase::setDebugUtilsObjectName(
        *m_graphicsCompQueue, "re::VulkanFixture::graphicsCompQueue"
    );

    // Initialize ImGui for SDL2
    if (!ImGui_ImplSDL2_InitForVulkan(m_sdlWindow)) {
        throw std::runtime_error{"Could not initialize ImGui-SDL2 for Vulkan!"};
    }
}

VulkanFixture::~VulkanFixture() {
    m_device.waitIdle();
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
}

void VulkanFixture::setMainRenderPass(const RenderPass& rp, uint32_t imGuiSubpassIndex) {
    if (m_mainRenderPass) {
        if (m_imGuiSubpassIndex != RoomDisplaySettings::k_notUsingImGui) {
            ImGui_ImplVulkan_Shutdown();
        }
        m_swapChainFramebuffers.~vector();
    }

    m_mainRenderPass    = &rp;
    m_imGuiSubpassIndex = imGuiSubpassIndex;

    new (&m_swapChainFramebuffers) decltype(m_swapChainFramebuffers
    ){createSwapchainFramebuffers()};

    if (m_imGuiSubpassIndex != RoomDisplaySettings::k_notUsingImGui) {
        // Initialize ImGui for the new renderpass
        ImGui_ImplVulkan_InitInfo imGuiInitInfo{
            .Instance       = *m_instance,
            .PhysicalDevice = *m_physicalDevice,
            .Device         = *m_device,
            .QueueFamily    = m_graphicsCompQueueFamIndex,
            .Queue          = *m_graphicsCompQueue,
            .DescriptorPool = *m_descriptorPool,
            .RenderPass     = **m_mainRenderPass,
            .MinImageCount  = m_minImageCount,
            .ImageCount  = static_cast<uint32_t>(m_swapchainImageViews.size()),
            .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
            .PipelineCache       = *m_pipelineCache,
            .Subpass             = imGuiSubpassIndex,
            .UseDynamicRendering = false,
            .Allocator           = nullptr,
            .CheckVkResultFn     = &checkSuccessImGui
        };
        if (!ImGui_ImplVulkan_Init(&imGuiInitInfo)) {
            throw std::runtime_error{"Could not initialize ImGui for Vulkan!"};
        }
    }
}

const CommandBuffer& VulkanFixture::prepareFrame() {
    // Wait for the previous frame to finish
    checkSuccess(m_device.waitForFences(**m_inFlightFences, true, k_maxTimeout));

    m_deletionQueue.deleteNextGroup();
    m_deletionQueue.beginNewGroup();

    // Recreate swapchain if required
    if (m_recreteSwapchain) {
        recreateSwapchain();
        m_recreteSwapchain = false;
    }

    m_device.resetFences(**m_inFlightFences);

    // Acquire next image
    vk::AcquireNextImageInfoKHR acquireNextImageInfo{
        *m_swapchain, k_maxTimeout, **m_imageAvailableSems, nullptr, 1u
    };
    auto [res, imageIndex] = m_device.acquireNextImage2KHR(acquireNextImageInfo);
    checkSuccess(res);
    m_imageIndex = imageIndex;

    // Restart command buffer
    auto& cb = *m_cbs;
    cb->reset();
    cb->begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});

    // Begin ImGui frame
    if (m_imGuiSubpassIndex != RoomDisplaySettings::k_notUsingImGui) {
        // Rebuild fonts if new fonts have been added
        if (!ImGui::GetIO().Fonts->IsBuilt()) {
            ImGui_ImplVulkan_CreateFontsTexture();
        }

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    return cb;
}

void VulkanFixture::mainRenderPassBegin(std::span<const vk::ClearValue> clearValues
) {
    auto& cb = m_cbs.write();
    cb->beginRenderPass2(
        vk::RenderPassBeginInfo{
            **m_mainRenderPass, *m_swapChainFramebuffers[m_imageIndex],
            vk::Rect2D{{}, m_swapchainExtent}, clearValues
        },
        vk::SubpassBeginInfo{vk::SubpassContents::eInline}
    );

    // Set default viewport & scissor
    glm::vec2 extent{m_swapchainExtent.width, m_swapchainExtent.height};
    cb->setViewport(
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
    cb->setScissor(
        0u,
        vk::Rect2D{
            {0, 0},                                             // x, y
            {m_swapchainExtent.width, m_swapchainExtent.height} // width, height
        }
    );
}

void VulkanFixture::mainRenderPassNextSubpass() {
    m_cbs.write()->nextSubpass2(
        vk::SubpassBeginInfo{vk::SubpassContents::eInline}, vk::SubpassEndInfo{}
    );
}

void VulkanFixture::mainRenderPassDrawImGui() {
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), **m_cbs);
}

void VulkanFixture::mainRenderPassEnd() {
    m_cbs.write()->endRenderPass2(vk::SubpassEndInfo{});
}

void VulkanFixture::finishFrame() {
    auto& cb = m_cbs.write();
    cb->end();

    // Submit the command buffer
    vk::PipelineStageFlags waitDstStageMask =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::SubmitInfo submitInfo{
        **m_imageAvailableSems,   // Wait for image to be available
        waitDstStageMask,         // Wait just before writing output
        *cb,
        **m_renderingFinishedSems // Signal that the rendering has
                                  // finished once done
    };
    m_graphicsCompQueue.submit(submitInfo, **m_inFlightFences);

    // Present new image
    vk::PresentInfoKHR presentInfo{
        **m_renderingFinishedSems, // Wait for rendering to finish
        *m_swapchain, m_imageIndex
    };

    try {
        checkSuccess(m_presentationQueue.presentKHR(presentInfo));
    } catch (vk::OutOfDateKHRError&) { recreateSwapchain(); }

    FrameDoubleBufferingState::setTotalIndex(m_frame++);
}

void VulkanFixture::changePresentation(bool vSync) {
    m_presentMode      = selectClosestPresentMode(vSync);
    m_recreteSwapchain = true;
}

void VulkanFixture::prepareForDestructionOfRendererObjects() {
    m_device.waitIdle();
}

std::vector<std::string> VulkanFixture::availableDevices() const {
    auto physDevices = m_instance.enumeratePhysicalDevices();
    std::vector<std::string> rval;
    rval.reserve(physDevices.size());
    for (const auto& physDevice : physDevices) {
        std::string temp = physDevice.getProperties2().properties.deviceName;
        rval.emplace_back(std::move(temp));
    }
    return rval;
}

std::string VulkanFixture::usedDevice() const {
    return m_physicalDevice.getProperties2().properties.deviceName;
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
        "VK_LAYER_KHRONOS_validation"
#endif // !NDEBUG
    };

    // Add extensions required by SDL2
    unsigned int sdl2ExtensionCount{};
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
        &debugMessengerCallbackHandler
    };
    return vk::raii::Instance{
        m_context,
        vk::InstanceCreateInfo{
            {}, &applicationInfo, validationLayers, extensions, &debugMessengerCreateInfo
        }
    };
}

vk::raii::DebugUtilsMessengerEXT VulkanFixture::createDebugUtilsMessenger() {
    vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{
        {},
        eVerbose | eInfo | eWarning | eError,
        eGeneral | eValidation | ePerformance,
        &debugMessengerCallbackHandler
    };
    return vk::raii::DebugUtilsMessengerEXT{m_instance, debugMessengerCreateInfo};
}

vk::raii::SurfaceKHR VulkanFixture::createSurface() {
    VkSurfaceKHR surface{};
    if (!SDL_Vulkan_CreateSurface(m_sdlWindow, *m_instance, &surface)) {
        throw std::runtime_error("SDL2 could not create Vulkan surface!");
    }
    return vk::raii::SurfaceKHR{m_instance, surface};
}

vk::raii::PhysicalDevice VulkanFixture::createPhysicalDevice(
    std::string_view preferredDevice, const void* deviceCreateInfoChain
) {
    SelectedPhysDevice res = selectSuitablePhysDevice(
        *m_instance,
        PhysDeviceRequirements{
            .surface               = *m_surface,
            .deviceCreateInfoChain = deviceCreateInfoChain,
            .preferredDevice       = preferredDevice
        }
    );

    // If a device was selected (= suitable)
    if (res) {
        // Save queue famili indices
        m_graphicsCompQueueFamIndex = res.graphicsCompQueueFamIndex;
        m_presentationQueueFamIndex = res.presentationQueueFamIndex;

        // Print device name
        auto props = res.selected.getProperties2().properties;
        auto major = vk::apiVersionMajor(props.apiVersion);
        auto minor = vk::apiVersionMinor(props.apiVersion);
        auto patch = vk::apiVersionPatch(props.apiVersion);
        std::cout << "Vulkan:       " << major << '.' << minor << '.' << patch
                  << '\n';
        std::cout << "Device:       " << props.deviceName << std::endl;
        return vk::raii::PhysicalDevice{m_instance, res.selected};
    }

    throw std::runtime_error("No physical device is suitable!");
}

vk::PresentModeKHR VulkanFixture::selectClosestPresentMode(bool vSync) {
    auto modes     = m_physicalDevice.getSurfacePresentModesKHR(*m_surface);
    auto idealMode = vSync ? eMailbox : eImmediate;
    auto acceptableMode  = vSync ? eFifo : eFifoRelaxed;
    bool idealAvail      = false;
    bool acceptableAvail = false;

    for (auto mode : modes) {
        if (mode == idealMode) {
            idealAvail = true;
        } else if (mode == acceptableMode) {
            acceptableAvail = true;
        }
    }

    return idealAvail ? idealMode : (acceptableAvail ? acceptableMode : eFifo);
}

vk::raii::Device VulkanFixture::createDevice(const void* deviceCreateInfoChain) {
    float deviceQueuePriority = 1.0f;
    std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos;
    deviceQueueCreateInfos.emplace_back(
        vk::DeviceQueueCreateFlags{}, m_graphicsCompQueueFamIndex, 1, &deviceQueuePriority
    );
    if (m_graphicsCompQueueFamIndex != m_presentationQueueFamIndex) {
        deviceQueueCreateInfos.emplace_back(
            vk::DeviceQueueCreateFlags{}, m_presentationQueueFamIndex, 1,
            &deviceQueuePriority
        );
    }
    vk::DeviceCreateInfo createInfo{{}, deviceQueueCreateInfos,
                                    {}, k_deviceExtensions,
                                    {}, deviceCreateInfoChain};

    return vk::raii::Device{m_physicalDevice, createInfo};
}

vma::Allocator VulkanFixture::createAllocator() {
    return vma::createAllocator(vma::AllocatorCreateInfo{
        vma::AllocatorCreateFlagBits::eExternallySynchronized,
        *m_physicalDevice, *m_device,
        0,       // Use default large heap block size (256 MB as of writing)
        nullptr, // Use default CPU memory allocation callbacks
        nullptr, // Do not receive informative callbacks from VMA
        nullptr, // No heap size limits
        nullptr, // Let VMA load Vulkan functions on its own
        *m_instance, vk::ApiVersion13,
        nullptr, // No external memory handles
    });
}

vk::raii::Queue VulkanFixture::getQueue(uint32_t familyIndex) {
    return vk::raii::Queue{m_device, familyIndex, 0u};
}

vk::raii::SwapchainKHR VulkanFixture::createSwapchain() {
    auto caps = m_physicalDevice.getSurfaceCapabilitiesKHR(*m_surface);
    // Minimum image count
    m_minImageCount = glm::clamp(
        caps.minImageCount + 1, caps.minImageCount,
        caps.maxImageCount ? caps.maxImageCount : 8u
    );

    // Extent
    if (caps.currentExtent == vk::Extent2D{0xFFFFFFFF, 0xFFFFFFFF}) {
        m_swapchainExtent = caps.currentExtent;
    } else {
        glm::ivec2 windowPx;
        SDL_Vulkan_GetDrawableSize(m_sdlWindow, &windowPx.x, &windowPx.y);
        m_swapchainExtent.width = std::clamp(
            static_cast<uint32_t>(windowPx.x), caps.minImageExtent.width,
            caps.maxImageExtent.width
        );
        m_swapchainExtent.height = std::clamp(
            static_cast<uint32_t>(windowPx.y), caps.minImageExtent.height,
            caps.maxImageExtent.height
        );
    }

    // Sharing mode
    bool oneQueueFamily = m_graphicsCompQueueFamIndex == m_presentationQueueFamIndex;
    auto sharingMode              = oneQueueFamily ? eExclusive : eConcurrent;
    std::array queueFamilyIndices = {
        m_graphicsCompQueueFamIndex, m_presentationQueueFamIndex
    };
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
        m_presentMode,
        true
    };
    return vk::raii::SwapchainKHR{m_device, createInfo};
}

std::vector<vk::raii::ImageView> VulkanFixture::createSwapchainImageViews() {
    auto images = m_swapchain.getImages();
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
                vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u}
            }
        );
    }
    return imageViews;
}

std::vector<Texture> VulkanFixture::createAdditionalBuffers() {
    // Ugly hack to be able to use re::Texture already
    VulkanObjectBase::s_allocator = &m_allocator;
    VulkanObjectBase::s_device    = &(*m_device);
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
            .hasSampler    = false
        });
    }
    return buffers;
}

std::vector<vk::raii::Framebuffer> VulkanFixture::createSwapchainFramebuffers() {
    std::vector<vk::ImageView> views;
    views.resize(1 + m_additionalBuffers.size());
    vk::FramebufferCreateInfo createInfo{
        {},
        **m_mainRenderPass,
        views,
        m_swapchainExtent.width,
        m_swapchainExtent.height,
        1
    };

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
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer, m_graphicsCompQueueFamIndex
    };
    return vk::raii::CommandPool{m_device, createInfo};
}

FrameDoubleBuffered<vk::raii::Semaphore> VulkanFixture::createSemaphores() {
    vk::SemaphoreCreateInfo createInfo{};
    return {
        vk::raii::Semaphore{m_device, createInfo},
        vk::raii::Semaphore{m_device, createInfo}
    };
}

FrameDoubleBuffered<vk::raii::Fence> VulkanFixture::createFences() {
    vk::FenceCreateInfo createInfo{vk::FenceCreateFlagBits::eSignaled};
    return {vk::raii::Fence{m_device, createInfo}, vk::raii::Fence{m_device, createInfo}};
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
        poolSizes
    };
    return vk::raii::DescriptorPool{m_device, createInfo};
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

void VulkanFixture::assignImplementationReferences() {
    VulkanObjectBase::s_physicalDevice        = &(*m_physicalDevice);
    VulkanObjectBase::s_device                = &(*m_device);
    VulkanObjectBase::s_allocator             = &m_allocator;
    VulkanObjectBase::s_graphicsCompQueue     = &(*m_graphicsCompQueue);
    VulkanObjectBase::s_commandPool           = &(*m_commandPool);
    VulkanObjectBase::s_descriptorPool        = &(*m_descriptorPool);
    VulkanObjectBase::s_pipelineCache         = &(*m_pipelineCache);
    VulkanObjectBase::s_oneTimeSubmitCmdBuf   = &m_oneTimeSubmitCmdBuf;
    VulkanObjectBase::s_dispatchLoaderDynamic = &(m_dispatchLoaderDynamic);
    VulkanObjectBase::s_deletionQueue         = &m_deletionQueue;
}

} // namespace re
