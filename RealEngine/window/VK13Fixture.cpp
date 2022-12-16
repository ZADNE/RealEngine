/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/window/VK13Fixture.hpp>

#include <bitset>
#include <iostream>

#include <SDL2/SDL_vulkan.h>
#include <ImGui/imgui_impl_sdl.h>
#include <ImGui/imgui_impl_vulkan.h>
#include <glm/common.hpp>

#include <RealEngine/window/WindowSubsystems.hpp>
#include <RealEngine/rendering/buffers/Buffer.hpp>
#include <RealEngine/rendering/DescriptorSet.hpp>
#include <RealEngine/rendering/Pipeline.hpp>
#include <RealEngine/rendering/textures/Texture.hpp>

using enum vk::DebugUtilsMessageSeverityFlagBitsEXT;
using enum vk::DebugUtilsMessageTypeFlagBitsEXT;
using enum vk::SharingMode;
using enum vk::ImageUsageFlagBits;
using enum vk::CompositeAlphaFlagBitsKHR;
using enum vk::PresentModeKHR;
using enum vk::ImageViewType;

namespace {
constexpr auto MAX_TIMEOUT = std::numeric_limits<uint64_t>::max();
void checkSuccess(vk::Result res) { assert(res == vk::Result::eSuccess); }
void checkSuccessImGui(VkResult res) { checkSuccess(vk::Result{res}); }
}

namespace RE {

constexpr std::array DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

constexpr vk::SurfaceFormatKHR SURFACE_FORMAT{
    vk::Format::eB8G8R8A8Unorm,
    vk::ColorSpaceKHR::eSrgbNonlinear
};

VK13Fixture::VK13Fixture(SDL_Window* sdlWindow, bool vSync) :
    m_sdlWindow(sdlWindow),
    m_vSync(vSync),
    //Vulkan objects
    m_instance(createInstance()),
#ifndef NDEBUG
    m_debugUtilsMessenger(createDebugUtilsMessenger()),
#endif // !NDEBUG
    m_surface(createSurface()),
    m_physicalDevice(createPhysicalDevice()),
    m_device(createDevice()),
    m_graphicsQueue(createQueue(m_graphicsQueueFamilyIndex)),
    m_presentationQueue(createQueue(m_presentationQueueFamilyIndex)),
    m_swapchain(createSwapchain()),
    m_swapchainImageViews(createSwapchainImageViews()),
    m_renderPass(createRenderPass()),
    m_swapChainFramebuffers(createSwapchainFramebuffers()),
    m_commandPool(createCommandPool()),
    m_commandBuffers(createCommandBuffers()),
    m_pipelineCache(createPipelineCache()),
    m_descriptorPool(createDescriptorPool()),
    m_imageAvailableSems(createSemaphores()),
    m_renderingFinishedSems(createSemaphores()),
    m_inFlightFences(createFences()) {
    //Implementations
    assignImplementationReferences();
    setFramesInFlight(2);
    setFrame(m_frame++);
    //Initialize ImGui
    if (!ImGui_ImplSDL2_InitForVulkan(m_sdlWindow)) {
        throw std::runtime_error{"Could not initialize ImGui-SDL2 for Vulkan!"};
    }
    ImGui_ImplVulkan_InitInfo initInfo{
        .Instance = *m_instance, .PhysicalDevice = *m_physicalDevice,
        .Device = *m_device, .QueueFamily = m_graphicsQueueFamilyIndex,
        .Queue = *m_graphicsQueue, .PipelineCache = *m_pipelineCache,
        .DescriptorPool = *m_descriptorPool, .Subpass = 0u,
        .MinImageCount = m_minImageCount, .ImageCount = static_cast<uint32_t>(m_swapchainImageViews.size()),
        .MSAASamples = VK_SAMPLE_COUNT_1_BIT, .Allocator = nullptr,
        .CheckVkResultFn = &checkSuccessImGui
    };
    if (!ImGui_ImplVulkan_Init(&initInfo, *m_renderPass)) {
        ImGui_ImplSDL2_Shutdown();
        throw std::runtime_error{"Could not initialize ImGui for Vulkan!"};
    }
}

VK13Fixture::~VK13Fixture() {
    m_device.waitIdle();
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    clearImplementationReferences();
}

const vk::CommandBuffer& VK13Fixture::prepareFrame(const glm::vec4& clearColor, bool useImGui) {
    //Wait for the previous frame to finish
    checkSuccess(m_device.waitForFences(*current(m_inFlightFences), true, MAX_TIMEOUT));

    //Recreate swapchain if required
    if (m_recreteSwapchain) {
        recreateSwapchain();
        m_recreteSwapchain = false;
    }

    m_device.resetFences(*current(m_inFlightFences));

    //Acquire next image
    vk::AcquireNextImageInfoKHR acquireNextImageInfo{
        *m_swapchain,
        MAX_TIMEOUT,
        *current(m_imageAvailableSems),
        nullptr,
        1u
    };
    auto [res, imageIndex] = m_device.acquireNextImage2KHR(acquireNextImageInfo);
    checkSuccess(res);
    m_imageIndex = imageIndex;

    //Rebuild fonts if new font were added
    if (!ImGui::GetIO().Fonts->IsBuilt()) {
        recreateImGuiFontTexture();
    }

    //Restart command buffer
    auto& commandBuffer = current(m_commandBuffers);
    commandBuffer.reset();
    commandBuffer.begin({});

    //Set current commandbuffer
    Buffer::s_commandBuffer = DescriptorSet::s_commandBuffer = Pipeline::s_commandBuffer = &(*commandBuffer);

    //Begin renderpass
    const auto* clearColorPtr = reinterpret_cast<const std::array<float, 4u>*>(&clearColor);
    vk::ClearValue clearValue{vk::ClearColorValue{*clearColorPtr}};
    vk::RenderPassBeginInfo renderPassBeginInfo{
        *m_renderPass,
        *m_swapChainFramebuffers[m_imageIndex],
        vk::Rect2D{{}, m_swapchainExtent},
        clearValue
    };
    commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    //Begin ImGui frame
    if (useImGui) {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    //Set default viewport & scissor
    glm::vec2 extent{m_swapchainExtent.width, m_swapchainExtent.height};
    commandBuffer.setViewport(0u, vk::Viewport{
        0.0f, extent.y,                                     //X, Y
        extent.x, -extent.y,                                //Width, height
        0.0f, 1.0f                                          //MinDepth, MaxDepth
        });
    commandBuffer.setScissor(0u, vk::Rect2D{
        {0, 0},                                             //X, Y
        {m_swapchainExtent.width, m_swapchainExtent.height} //Width, height
        });

    return (*commandBuffer);
}

void VK13Fixture::finishFrame(bool useImGui) {
    auto& commandBuffer = current(m_commandBuffers);
    if (useImGui) {
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *commandBuffer);
    }

    //Submit the command buffer
    commandBuffer.endRenderPass();
    commandBuffer.end();
    vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::SubmitInfo submitInfo{
        *current(m_imageAvailableSems),
        waitDstStageMask,
        *commandBuffer,
        *current(m_renderingFinishedSems)
    };
    m_graphicsQueue.submit(submitInfo, *current(m_inFlightFences));

    //Present new image
    vk::PresentInfoKHR presentInfo{
        *current(m_renderingFinishedSems),
        *m_swapchain,
        m_imageIndex
    };

    try {
        checkSuccess(m_presentationQueue.presentKHR(presentInfo));
    }
    catch (vk::OutOfDateKHRError&) {
        recreateSwapchain();
    }

    setFrame(m_frame++);
}

void VK13Fixture::changePresentation(bool vSync) {
    m_vSync = vSync;
    m_recreteSwapchain = true;
}

void VK13Fixture::prepareForDestructionOfRendererObjects() {
    m_device.waitIdle();
}

vk::raii::Instance VK13Fixture::createInstance() {
    //Prepare default layers and extensions
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

    //Add extensions required by SDL2
    unsigned int sdl2ExtensionCount;
    if (!SDL_Vulkan_GetInstanceExtensions(m_sdlWindow, &sdl2ExtensionCount, nullptr)) {
        throw std::runtime_error("Could not get number of Vulkan extensions required for SDL2!");
    }
    size_t defaultExtensionsCount = extensions.size();
    extensions.resize(defaultExtensionsCount + sdl2ExtensionCount);
    if (!SDL_Vulkan_GetInstanceExtensions(m_sdlWindow, &sdl2ExtensionCount, &extensions[defaultExtensionsCount])) {
        throw std::runtime_error("Could not get Vulkan extensions required for SDL2!");
    }

    //Create Vulkan instance
    vk::ApplicationInfo applicationInfo("RealEngine", 1, "RealEngine", RE_VERSION, VK_API_VERSION_1_3);
    vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{{},
        /*eVerbose | eInfo |*/ eWarning | eError,
        eGeneral | eValidation | ePerformance,
        &VK13Fixture::debugMessengerCallback
    };
    return vk::raii::Instance{m_context, vk::InstanceCreateInfo{{}, &applicationInfo, validationLayers, extensions, &debugMessengerCreateInfo}};
}

vk::raii::DebugUtilsMessengerEXT VK13Fixture::createDebugUtilsMessenger() {
    vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{{},
        /*eVerbose | eInfo |*/ eWarning | eError,
        eGeneral | eValidation | ePerformance,
        &VK13Fixture::debugMessengerCallback
    };
    return vk::raii::DebugUtilsMessengerEXT{m_instance, debugMessengerCreateInfo};
}

vk::raii::SurfaceKHR VK13Fixture::createSurface() {
    VkSurfaceKHR surface;
    if (!SDL_Vulkan_CreateSurface(m_sdlWindow, *m_instance, &surface)) {
        throw std::runtime_error("SDL2 could not create Vulkan surface!");
    }
    return vk::raii::SurfaceKHR{m_instance, surface};
}

vk::raii::PhysicalDevice VK13Fixture::createPhysicalDevice() {
    for (const auto& physicalDevice : vk::raii::PhysicalDevices{m_instance}) {//Iterate over all physical device
        if (areExtensionsSupported(physicalDevice) && isSwapchainSupported(physicalDevice) && findQueueFamilyIndices(physicalDevice)) {
            auto props = physicalDevice.getProperties2();
            auto major = VK_API_VERSION_MAJOR(props.properties.apiVersion);
            auto minor = VK_API_VERSION_MINOR(props.properties.apiVersion);
            if (major >= 1 && minor >= 2) {
                std::cout << "Vulkan:       " << major << '.' << minor << std::endl;
                return physicalDevice;
            }
        }
    }
    throw std::runtime_error("No physical device is suitable!");
}

vk::raii::Device VK13Fixture::createDevice() {
    float deviceQueuePriority = 1.0f;
    std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos;
    deviceQueueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags{}, m_graphicsQueueFamilyIndex, 1, &deviceQueuePriority);
    if (m_graphicsQueueFamilyIndex != m_presentationQueueFamilyIndex) {
        deviceQueueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags{}, m_presentationQueueFamilyIndex, 1, &deviceQueuePriority);
    }
    return vk::raii::Device{m_physicalDevice, vk::DeviceCreateInfo{vk::DeviceCreateFlags{}, deviceQueueCreateInfos, {}, DEVICE_EXTENSIONS}};
}

vk::raii::Queue VK13Fixture::createQueue(uint32_t familyIndex) {
    return vk::raii::Queue{m_device, familyIndex, 0u};
}

vk::raii::SwapchainKHR VK13Fixture::createSwapchain() {
    auto caps = m_physicalDevice.getSurfaceCapabilitiesKHR(*m_surface);
    //Minimum image count
    m_minImageCount = glm::clamp(
        caps.minImageCount + 1,
        caps.minImageCount,
        caps.maxImageCount ? caps.maxImageCount : 8
    );

    //Extent
    if (caps.currentExtent == vk::Extent2D{0xFFFFFFFF, 0xFFFFFFFF}) {
        m_swapchainExtent = caps.currentExtent;
    } else {
        glm::ivec2 windowPx;
        SDL_Vulkan_GetDrawableSize(m_sdlWindow, &windowPx.x, &windowPx.y);
        m_swapchainExtent.width = std::clamp(static_cast<uint32_t>(windowPx.x), caps.minImageExtent.width, caps.maxImageExtent.width);
        m_swapchainExtent.height = std::clamp(static_cast<uint32_t>(windowPx.y), caps.minImageExtent.height, caps.maxImageExtent.height);
    }

    //Sharing mode
    bool oneQueueFamily = m_graphicsQueueFamilyIndex == m_presentationQueueFamilyIndex;
    auto sharingMode = oneQueueFamily ? eExclusive : eConcurrent;
    std::array queueFamilyIndices = {m_graphicsQueueFamilyIndex, m_presentationQueueFamilyIndex};
    vk::SwapchainCreateInfoKHR createInfo{{}, *m_surface, m_minImageCount,
        SURFACE_FORMAT.format, SURFACE_FORMAT.colorSpace,
        m_swapchainExtent, 1u, eColorAttachment,
        sharingMode, oneQueueFamily ? vk::ArrayProxyNoTemporaries<const uint32_t>{} : queueFamilyIndices,
        caps.currentTransform, eOpaque,
        m_vSync ? eMailbox : eImmediate, true
    };
    return vk::raii::SwapchainKHR{m_device, createInfo};
}

std::vector<vk::raii::ImageView> VK13Fixture::createSwapchainImageViews() {
    auto images = m_swapchain.getImages();
    std::vector<vk::raii::ImageView> imageViews;
    imageViews.reserve(images.size());
    for (const auto& image : images) {
        imageViews.emplace_back(m_device, vk::ImageViewCreateInfo{{},
            image, e2D, SURFACE_FORMAT.format, {},
            vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u}}
        );
    }
    return imageViews;
}

vk::raii::RenderPass VK13Fixture::createRenderPass() {
    vk::AttachmentDescription attachmentDescription{{},
        SURFACE_FORMAT.format,
        vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eClear,       //Color
        vk::AttachmentStoreOp::eStore,      //Color
        vk::AttachmentLoadOp::eDontCare,    //Stencil
        vk::AttachmentStoreOp::eDontCare,   //Stencil
        vk::ImageLayout::eUndefined,        //Initial
        vk::ImageLayout::ePresentSrcKHR     //Final
    };
    vk::AttachmentReference attachmentRef{
        0, vk::ImageLayout::eColorAttachmentOptimal
    };
    vk::SubpassDescription subpassDescription{{},
        vk::PipelineBindPoint::eGraphics,
        {},                 //Input attachments
        attachmentRef       //Color attachments
    };
    vk::SubpassDependency subpassDependency{
        VK_SUBPASS_EXTERNAL,                                //Src subpass
        0u,                                                 //Dst subpass
        {vk::PipelineStageFlagBits::eColorAttachmentOutput},//Src stage mask
        {vk::PipelineStageFlagBits::eColorAttachmentOutput},//Dst stage mask
        vk::AccessFlags{},                                  //Src access mask
        {vk::AccessFlagBits::eColorAttachmentWrite}         //Dst access mask
    };
    vk::RenderPassCreateInfo createInfo{{},
        attachmentDescription,
        subpassDescription,
        subpassDependency
    };
    return vk::raii::RenderPass{m_device, createInfo};
}

std::vector<vk::raii::Framebuffer> VK13Fixture::createSwapchainFramebuffers() {
    vk::FramebufferCreateInfo createInfo{{},
        *m_renderPass, {},
        m_swapchainExtent.width, m_swapchainExtent.height, 1
    };
    std::vector<vk::raii::Framebuffer> vec;
    vec.reserve(m_swapchainImageViews.size());
    for (size_t i = 0; i < m_swapchainImageViews.size(); i++) {
        createInfo.setAttachments(*m_swapchainImageViews[i]);
        vec.emplace_back(m_device, createInfo);
    }
    return vec;
}

vk::raii::CommandPool VK13Fixture::createCommandPool() {
    vk::CommandPoolCreateInfo createInfo{
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        m_graphicsQueueFamilyIndex
    };
    return vk::raii::CommandPool{m_device, createInfo};
}

PerFrameInFlight<vk::raii::CommandBuffer> VK13Fixture::createCommandBuffers() {
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo{*m_commandPool, vk::CommandBufferLevel::ePrimary, MAX_FRAMES_IN_FLIGHT};
    vk::raii::CommandBuffers buffers{m_device, commandBufferAllocateInfo};
    assert(buffers.size() == MAX_FRAMES_IN_FLIGHT);
    return PerFrameInFlight<vk::raii::CommandBuffer>{std::move(buffers[0]), std::move(buffers[1])};
}

PerFrameInFlight<vk::raii::Semaphore> VK13Fixture::createSemaphores() {
    vk::SemaphoreCreateInfo createInfo{};
    return {
        vk::raii::Semaphore{m_device, createInfo},
        vk::raii::Semaphore{m_device, createInfo}
    };
}

PerFrameInFlight<vk::raii::Fence> VK13Fixture::createFences() {
    vk::FenceCreateInfo createInfo{vk::FenceCreateFlagBits::eSignaled};
    return {
        vk::raii::Fence{m_device, createInfo},
        vk::raii::Fence{m_device, createInfo}
    };
}

vk::raii::PipelineCache VK13Fixture::createPipelineCache() {
    return vk::raii::PipelineCache{m_device, vk::PipelineCacheCreateInfo{}};
}

vk::raii::DescriptorPool VK13Fixture::createDescriptorPool() {
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
    vk::DescriptorPoolCreateInfo createInfo{{},
        static_cast<uint32_t>(m_swapchainImageViews.size()) * 8u, //'8 is just enough' - needs more work
        poolSizes
    };
    return vk::raii::DescriptorPool{m_device, createInfo};
}

VkBool32 VK13Fixture::debugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT sev, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData) {
    std::cerr << callbackData->pMessage << '\n';
    return false;
}

bool VK13Fixture::areExtensionsSupported(const vk::raii::PhysicalDevice& physicalDevice) {
    std::bitset<DEVICE_EXTENSIONS.size()> supported{};
    for (const auto& extensionProperties : physicalDevice.enumerateDeviceExtensionProperties()) {
        for (size_t i = 0; i < DEVICE_EXTENSIONS.size(); ++i) {
            if (strcmp(extensionProperties.extensionName.data(), DEVICE_EXTENSIONS[i]) == 0) {
                supported[i] = true;
            }
        }
    }
    return supported.all();
}

bool VK13Fixture::isSwapchainSupported(const vk::raii::PhysicalDevice& physicalDevice) {
    bool formatSupported = false;
    for (const auto& format : physicalDevice.getSurfaceFormatsKHR(*m_surface)) {
        if (format == SURFACE_FORMAT) {
            formatSupported = true; break;
        }
    }
    bool mailboxSupported = false;
    for (const auto& presentMode : physicalDevice.getSurfacePresentModesKHR(*m_surface)) {
        if (presentMode == vk::PresentModeKHR::eMailbox) {
            mailboxSupported = true; break;
        }
    }
    return formatSupported && mailboxSupported;
}

bool VK13Fixture::findQueueFamilyIndices(const vk::raii::PhysicalDevice& physicalDevice) {
    bool graphicsQueueFound = false;
    bool presentQueueFound = false;
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++) {//Iterate over all queue families
        if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            m_graphicsQueueFamilyIndex = i;
            graphicsQueueFound = true;
        }
        if (physicalDevice.getSurfaceSupportKHR(i, *m_surface)) {
            m_presentationQueueFamilyIndex = i;
            presentQueueFound = true;
        }
        if (graphicsQueueFound && presentQueueFound) {
            return true;
        }
    }
    return false;
}

void VK13Fixture::recreateSwapchain() {
    m_device.waitIdle();
    m_swapChainFramebuffers.~vector();
    m_swapchainImageViews.~vector();
    m_swapchain.~SwapchainKHR();
    new (&m_swapchain) decltype(m_swapchain){createSwapchain()};
    new (&m_swapchainImageViews) decltype(m_swapchainImageViews){createSwapchainImageViews()};
    new (&m_swapChainFramebuffers) decltype(m_swapChainFramebuffers){createSwapchainFramebuffers()};
}

void VK13Fixture::recreateImGuiFontTexture() {
    vk::raii::Fence uploadFence{m_device, vk::FenceCreateInfo{}};
    current(m_commandBuffers).begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    ImGui_ImplVulkan_CreateFontsTexture(*current(m_commandBuffers));
    current(m_commandBuffers).end();
    m_graphicsQueue.submit(vk::SubmitInfo{{}, {}, *current(m_commandBuffers)}, *uploadFence);
    checkSuccess(m_device.waitForFences(*uploadFence, true, MAX_TIMEOUT));
}

void VK13Fixture::assignImplementationReferences() {
    Buffer::s_physicalDevice = Texture::s_physicalDevice = &(*m_physicalDevice);
    Buffer::s_device = Texture::s_device = DescriptorSet::s_device = Pipeline::s_device = &(*m_device);
    Buffer::s_graphicsQueue = Texture::s_graphicsQueue = &(*m_graphicsQueue);
    Buffer::s_commandPool = Texture::s_commandPool = &(*m_commandPool);
    DescriptorSet::s_descriptorPool = &(*m_descriptorPool);
    Pipeline::s_pipelineCache = &(*m_pipelineCache);
    Pipeline::s_renderPass = &(*m_renderPass);
}

void VK13Fixture::clearImplementationReferences() {
    Buffer::s_physicalDevice = Texture::s_physicalDevice = nullptr;
    Buffer::s_device = Texture::s_device = DescriptorSet::s_device = Pipeline::s_device = nullptr;
    Buffer::s_graphicsQueue = Texture::s_graphicsQueue = nullptr;
    Buffer::s_commandPool = Texture::s_commandPool = nullptr;
    DescriptorSet::s_descriptorPool = nullptr;
    Pipeline::s_pipelineCache = nullptr;
    Pipeline::s_renderPass = nullptr;
}

}