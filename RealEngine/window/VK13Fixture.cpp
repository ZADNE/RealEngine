/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/window/VK13Fixture.hpp>

#include <bitset>
#include <iostream>

#include <SDL2/SDL_vulkan.h>

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

namespace RE {

std::optional<VK13Fixture> VK13Fixture::s_fixture{};

static constexpr std::array DEVICE_EXTENSIONS = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

bool VK13Fixture::initialize(SDL_Window* sdlWindow, const glm::ivec2& windowPx, bool vSync) {
    try {
        s_fixture.emplace(sdlWindow, windowPx, vSync);
    }
    catch (std::exception&) {
        return false;
    }
    return true;
}

VK13Fixture::VK13Fixture(SDL_Window* sdlWindow, const glm::ivec2& windowPx, bool vSync) :
    m_instance(createInstance(sdlWindow)),
#ifndef NDEBUG
    m_debugUtilsMessenger(createDebugUtilsMessenger()),
#endif // !NDEBUG
    m_surface(createSurface(sdlWindow)),
    m_physicalDevice(createPhysicalDevice()),
    m_device(createDevice()),
    m_graphicsQueue(createQueue(m_graphicsQueueFamilyIndex)),
    m_presentationQueue(createQueue(m_presentationQueueFamilyIndex)),
    m_swapchain(createSwapchain()),
    m_commandPool(createCommandPool()),
    m_commandBuffer(createCommandBuffer()) {

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
    throw std::runtime_error("No physical device is suitable");
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
    auto capabilities = m_physicalDevice.getSurfaceCapabilitiesKHR(*m_surface);
    std::cout << capabilities.currentExtent.width << ' ' << capabilities.currentExtent.height;
    vk::SwapchainCreateInfoKHR createInfo{};
    return vk::raii::SwapchainKHR{m_device, createInfo};
}

vk::raii::CommandPool VK13Fixture::createCommandPool() {
    return vk::raii::CommandPool{m_device, vk::CommandPoolCreateInfo{vk::CommandPoolCreateFlags{}, m_graphicsQueueFamilyIndex}};
}

vk::raii::CommandBuffer VK13Fixture::createCommandBuffer() {
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo{*m_commandPool, vk::CommandBufferLevel::ePrimary, 1};
    return vk::raii::CommandBuffer{std::move(vk::raii::CommandBuffers{m_device, commandBufferAllocateInfo}.front())};
}

VkBool32 VK13Fixture::debugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT sev, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData) {
    std::cout << callbackData->pMessage << '\n';
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
    return true;
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