/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/window/VK13Fixture.hpp>

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

namespace RE {

std::optional<VK13Fixture> VK13Fixture::s_fixture{};

constexpr vk::ApplicationInfo APP_INFO("RealEngine", 1, "RealEngine", RE_VERSION, VK_API_VERSION_1_3);

bool VK13Fixture::initialize(SDL_Window* sdlWindow) {
    try {
        s_fixture.emplace(sdlWindow);
    } catch (std::exception&) {
        return false;
    }
    return true;
}

VK13Fixture::VK13Fixture(SDL_Window* sdlWindow):
    m_instance(createInstance(sdlWindow)),
    m_device(createDevice()),
    m_commandPool(createCommandPool()),
    m_commandBuffer(createCommandBuffer()),
    m_surface(createSurface(sdlWindow)) {
    
}

vk::raii::Instance VK13Fixture::createInstance(SDL_Window* sdlWindow) {
    //Get number of required extensions
    unsigned int instanceExtensionCount;
    if (!SDL_Vulkan_GetInstanceExtensions(sdlWindow, &instanceExtensionCount, nullptr)) {
        throw std::runtime_error("Could not get number of Vulkan extensions required for SDL2!");
    }
    std::vector<const char*> instanceExtensions{instanceExtensionCount};
    if (!SDL_Vulkan_GetInstanceExtensions(sdlWindow, &instanceExtensionCount, instanceExtensions.data())) {
        throw std::runtime_error("Could not get Vulkan extensions required for SDL2!");
    }
    //Create Vulkan instance
    return {m_context, vk::InstanceCreateInfo{{}, &APP_INFO, {}, instanceExtensions}};
}

vk::raii::Device VK13Fixture::createDevice() {
    //Select physical device and its queue
    vk::raii::PhysicalDevice physicalDevice = m_instance.enumeratePhysicalDevices().front();
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    auto propertyIterator = std::find_if(queueFamilyProperties.begin(),
                                            queueFamilyProperties.end(),
                                            [](vk::QueueFamilyProperties const& qfp) { return qfp.queueFlags & vk::QueueFlagBits::eGraphics; });
    m_graphicsQueueFamilyIndex = std::distance(queueFamilyProperties.begin(), propertyIterator);
    assert(m_graphicsQueueFamilyIndex < queueFamilyProperties.size());

    //Create device
    float deviceQueuePriority = 1.0f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), static_cast<uint32_t>(m_graphicsQueueFamilyIndex), 1, &deviceQueuePriority);

    return {physicalDevice, vk::DeviceCreateInfo{vk::DeviceCreateFlags(), deviceQueueCreateInfo}};
}

vk::raii::CommandPool VK13Fixture::createCommandPool() {
    return {m_device, vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlags(), m_graphicsQueueFamilyIndex)};
}

vk::raii::CommandBuffer VK13Fixture::createCommandBuffer() {
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo{*m_commandPool, vk::CommandBufferLevel::ePrimary, 1};
    return {std::move(vk::raii::CommandBuffers{m_device, commandBufferAllocateInfo}.front())};
}

vk::raii::SurfaceKHR VK13Fixture::createSurface(SDL_Window* sdlWindow) {
    VkSurfaceKHR surface;
    if (!SDL_Vulkan_CreateSurface(sdlWindow, *m_instance, reinterpret_cast<VkSurfaceKHR*>(&surface))) {
        throw std::runtime_error("SDL2 could not create Vulkan surface!");
    }
    return {m_instance, surface};
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