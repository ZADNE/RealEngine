/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/window/VK13Fixture.hpp>

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

bool VK13Fixture::initialize(SDL_Window* sdlWindow) {
    try {
        s_fixture.emplace(sdlWindow);
    }
    catch (std::exception&) {
        return false;
    }
    return true;
}

VK13Fixture::VK13Fixture(SDL_Window* sdlWindow) :
    m_instance(createInstance(sdlWindow)),
#ifndef NDEBUG
    m_debugUtilsMessenger(createDebugUtilsMessenger()),
#endif // !NDEBUG
    m_physicalDevice(createPhysicalDevice()),
    m_device(createDevice()),
    m_commandPool(createCommandPool()),
    m_commandBuffer(createCommandBuffer()),
    m_surface(createSurface(sdlWindow)),
    m_swapchain(createSwapcahin()) {

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

vk::raii::PhysicalDevice VK13Fixture::createPhysicalDevice() {
    return vk::raii::PhysicalDevices{m_instance}.front();
}

vk::raii::Device VK13Fixture::createDevice() {
    //Select queue of the physical device
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_physicalDevice.getQueueFamilyProperties();
    auto propertyIterator = std::find_if(queueFamilyProperties.begin(),
                                            queueFamilyProperties.end(),
                                            [](vk::QueueFamilyProperties const& qfp) { return qfp.queueFlags & vk::QueueFlagBits::eGraphics; });
    m_graphicsQueueFamilyIndex = static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), propertyIterator));
    assert(m_graphicsQueueFamilyIndex < queueFamilyProperties.size());

    //Create device
    float deviceQueuePriority = 1.0f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), static_cast<uint32_t>(m_graphicsQueueFamilyIndex), 1, &deviceQueuePriority);
    return vk::raii::Device{m_physicalDevice, vk::DeviceCreateInfo{vk::DeviceCreateFlags(), deviceQueueCreateInfo}};
}

vk::raii::CommandPool VK13Fixture::createCommandPool() {
    return vk::raii::CommandPool{m_device, vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlags(), m_graphicsQueueFamilyIndex)};
}

vk::raii::CommandBuffer VK13Fixture::createCommandBuffer() {
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo{*m_commandPool, vk::CommandBufferLevel::ePrimary, 1};
    return vk::raii::CommandBuffer{std::move(vk::raii::CommandBuffers{m_device, commandBufferAllocateInfo}.front())};
}

vk::raii::SurfaceKHR VK13Fixture::createSurface(SDL_Window* sdlWindow) {
    VkSurfaceKHR surface;
    if (!SDL_Vulkan_CreateSurface(sdlWindow, *m_instance, &surface)) {
        throw std::runtime_error("SDL2 could not create Vulkan surface!");
    }
    return vk::raii::SurfaceKHR{m_instance, surface};
}

vk::raii::SwapchainKHR VK13Fixture::createSwapcahin() {
    return vk::raii::SwapchainKHR{m_device, {}};
}

VkBool32 VK13Fixture::debugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT sev, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData) {
    std::cout << callbackData->pMessage << '\n';
    return VK_FALSE;
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