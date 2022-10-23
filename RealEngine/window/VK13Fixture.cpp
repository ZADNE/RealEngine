/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/window/VK13Fixture.hpp>

#include <bitset>
#include <iostream>

#include <SDL2/SDL_vulkan.h>
#include <ImGui/imgui_impl_sdl.h>
#include <ImGui/imgui_impl_vulkan.h>

#include <RealEngine/window/WindowSubsystems.hpp>
#include <RealEngine/rendering/buffers/Buffer.hpp>
#include <RealEngine/rendering/Capabilities.hpp>
#include <RealEngine/rendering/output/Framebuffer.hpp>
#include <RealEngine/rendering/Ordering.hpp>
#include <RealEngine/rendering/vertices/ShaderProgram.hpp>
#include <RealEngine/rendering/textures/Texture.hpp>
#include <RealEngine/rendering/vertices/VertexArray.hpp>
#include <RealEngine/rendering/output/Viewport.hpp>

using enum vk::DebugUtilsMessageSeverityFlagBitsEXT;
using enum vk::DebugUtilsMessageTypeFlagBitsEXT;
using enum vk::SharingMode;
using enum vk::ImageUsageFlagBits;
using enum vk::CompositeAlphaFlagBitsKHR;
using enum vk::PresentModeKHR;
using enum vk::ImageViewType;

namespace RE {

constexpr std::array DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

constexpr vk::SurfaceFormatKHR SURFACE_FORMAT{
    vk::Format::eB8G8R8A8Srgb,
    vk::ColorSpaceKHR::eSrgbNonlinear
};

VK13Fixture::VK13Fixture(SDL_Window* sdlWindow, bool vSync) :
    m_instance(createInstance(sdlWindow)),
#ifndef NDEBUG
    m_debugUtilsMessenger(createDebugUtilsMessenger()),
#endif // !NDEBUG
    m_surface(createSurface(sdlWindow)),
    m_physicalDevice(createPhysicalDevice()),
    m_device(createDevice()),
    m_graphicsQueue(createQueue(m_graphicsQueueFamilyIndex)),
    m_presentationQueue(createQueue(m_presentationQueueFamilyIndex)),
    m_swapchain(createSwapchain(sdlWindow, vSync)),
    m_swapchainImageViews(createSwapchainImageViews()),
    m_renderPass(createRenderPass()),
    m_swapChainFramebuffers(createSwapchainFramebuffers()),
    m_commandPool(createCommandPool()),
    m_commandBuffer(createCommandBuffer()),
    m_pipelineCache(createPipelineCache()),
    m_descriptorPool(createDescriptorPool()) {
    //Initialize ImGui
    if (!ImGui_ImplSDL2_InitForVulkan(sdlWindow)){
        throw std::runtime_error{"Could not initialize ImGui-SDL2 for Vulkan!"};
    }
    ImGui_ImplVulkan_InitInfo initInfo{
        .Instance = *m_instance,
        .PhysicalDevice = *m_physicalDevice,
        .Device = *m_device,
        .QueueFamily = m_graphicsQueueFamilyIndex,
        .Queue = *m_graphicsQueue,
        .PipelineCache = *m_pipelineCache,
        .DescriptorPool = *m_descriptorPool,
        .Subpass = 0u,
        .MinImageCount = m_minImageCount,
        .ImageCount = static_cast<uint32_t>(m_swapchainImageViews.size()),
        .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
        .Allocator = nullptr,
        .CheckVkResultFn = nullptr
    };
    if (!ImGui_ImplVulkan_Init(&initInfo, *m_renderPass)) {
        ImGui_ImplSDL2_Shutdown();
        throw std::runtime_error{"Could not initialize ImGui for Vulkan!"};
    }
}

VK13Fixture::~VK13Fixture() {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
}

void VK13Fixture::prepareFrame(bool useImGui) {
    if (useImGui) {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }
}

void VK13Fixture::finishFrame(bool useImGui) {
    if (useImGui) {
        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *m_commandBuffer);
    }
}

vk::raii::Instance VK13Fixture::createInstance(SDL_Window* sdlWindow) {
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
    if (!SDL_Vulkan_GetInstanceExtensions(sdlWindow, &sdl2ExtensionCount, nullptr)) {
        throw std::runtime_error("Could not get number of Vulkan extensions required for SDL2!");
    }
    size_t defaultExtensionsCount = extensions.size();
    extensions.resize(defaultExtensionsCount + sdl2ExtensionCount);
    if (!SDL_Vulkan_GetInstanceExtensions(sdlWindow, &sdl2ExtensionCount, &extensions[defaultExtensionsCount])) {
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

vk::raii::SurfaceKHR VK13Fixture::createSurface(SDL_Window* sdlWindow) {
    VkSurfaceKHR surface;
    if (!SDL_Vulkan_CreateSurface(sdlWindow, *m_instance, &surface)) {
        throw std::runtime_error("SDL2 could not create Vulkan surface!");
    }
    return vk::raii::SurfaceKHR{m_instance, surface};
}

vk::raii::PhysicalDevice VK13Fixture::createPhysicalDevice() {
    for (const auto& physicalDevice : vk::raii::PhysicalDevices{m_instance}) {//Iterate over all physical device
        if (areExtensionsSupported(physicalDevice) && isSwapchainSupported(physicalDevice) && findQueueFamilyIndices(physicalDevice)) {
            return physicalDevice;
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

vk::raii::SwapchainKHR VK13Fixture::createSwapchain(SDL_Window* sdlWindow, bool vSync) {
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
        SDL_Vulkan_GetDrawableSize(sdlWindow, &windowPx.x, &windowPx.y);
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
        vSync ? eMailbox : eImmediate, true
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
        SURFACE_FORMAT.format, vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eDontCare,    //Color
        vk::AttachmentStoreOp::eStore,      //Color
        vk::AttachmentLoadOp::eDontCare,    //Stencil
        vk::AttachmentStoreOp::eDontCare,   //Stencil
        vk::ImageLayout::eUndefined,        //Initial
        vk::ImageLayout::ePresentSrcKHR     //Final
    };
    vk::AttachmentReference attachmentRef{
        0, vk::ImageLayout::eColorAttachmentOptimal
    };
    vk::SubpassDescription subPassDescription{{}, 
        vk::PipelineBindPoint::eGraphics,
        {},                 //Input attachments
        attachmentRef       //Color attachments
    };
    vk::RenderPassCreateInfo createInfo{{},
        attachmentDescription,
        subPassDescription
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

vk::raii::CommandBuffer VK13Fixture::createCommandBuffer() {
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo{*m_commandPool, vk::CommandBufferLevel::ePrimary, 1};
    return vk::raii::CommandBuffer{std::move(vk::raii::CommandBuffers{m_device, commandBufferAllocateInfo}.front())};
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
        static_cast<uint32_t>(m_swapchainImageViews.size()),
        poolSizes
    };
    return vk::raii::DescriptorPool{m_device, createInfo};
}

VkBool32 VK13Fixture::debugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT sev, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData) {
    std::cerr << callbackData->pMessage << '\n';
    return VK_FALSE;
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
    for (size_t i = 0; i < queueFamilyProperties.size(); i++) {//Iterate over all queue families
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

template<Renderer R>
void VK13Fixture::Implementations::assignReferences() {
    Buffer<R>::s_impl = &m_bufferImpl;
    Capabilities<R>::s_impl = &m_capabilitiesImpl;
    Framebuffer<R>::s_impl = &m_mainFramebufferImpl;
    Ordering<R>::s_impl = &m_orderingImpl;
    ShaderProgram<R>::s_impl = &m_shaderProgramImpl;
    Texture<R>::s_impl = &m_textureImpl;
    TextureProxy<R>::s_impl = &m_textureImpl;
    VertexArray<R>::s_impl = &m_vertexArrayImpl;
    Viewport<R>::s_impl = &m_viewportImpl;
    Viewport<R>::s_state = &m_viewportState;
}

template<Renderer R>
void VK13Fixture::Implementations::clearReferences() {
    Buffer<R>::s_impl = nullptr;
    Capabilities<R>::s_impl = nullptr;
    Framebuffer<R>::s_impl = nullptr;
    Ordering<R>::s_impl = nullptr;
    ShaderProgram<R>::s_impl = nullptr;
    Texture<R>::s_impl = nullptr;
    TextureProxy<R>::s_impl = nullptr;
    VertexArray<R>::s_impl = nullptr;
    Viewport<R>::s_impl = nullptr;
    Viewport<R>::s_state = nullptr;
}

}