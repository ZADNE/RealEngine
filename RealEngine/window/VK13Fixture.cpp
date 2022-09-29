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

bool VK13Fixture::prepare(SDL_Window* sdlWindow) {
    s_fixture.emplace(sdlWindow);

    return true;
}

void VK13Fixture::initialize() {

}

VK13Fixture::VK13Fixture(SDL_Window* sdlWindow) {
    //Prepare helper structs/variables
    unsigned int instanceExtensionCount;
    vk::ApplicationInfo applicationInfo("RealEngine", 1, "RealEngine", RE_VERSION, VK_API_VERSION_1_3);
    vk::InstanceCreateInfo instanceCreateInfo({}, &applicationInfo);
    vk::PhysicalDevice physicalDevice;
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties;
    float deviceQueuePriority = 0.0f;

    if (!SDL_Vulkan_GetInstanceExtensions(sdlWindow, &instanceExtensionCount, nullptr)) {
        goto fail_SDLWindow;
    }

    //Create Vulkan instance
    m_instance = vk::createInstance(instanceCreateInfo);

    //Select physical device and its queue
    physicalDevice = m_instance.enumeratePhysicalDevices().front();
    queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    auto propertyIterator = std::find_if(queueFamilyProperties.begin(),
                                            queueFamilyProperties.end(),
                                            [](vk::QueueFamilyProperties const& qfp) { return qfp.queueFlags & vk::QueueFlagBits::eGraphics; });
    size_t graphicsQueueFamilyIndex = std::distance(queueFamilyProperties.begin(), propertyIterator);
    assert(graphicsQueueFamilyIndex < queueFamilyProperties.size());

    //Create device
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), static_cast<uint32_t>(graphicsQueueFamilyIndex), 1, &deviceQueuePriority);
    m_device = physicalDevice.createDevice(vk::DeviceCreateInfo(vk::DeviceCreateFlags(), deviceQueueCreateInfo));

    //Create commnad pool & commnad buffer
    m_commandPool = m_device.createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlags(), graphicsQueueFamilyIndex));
    m_commandBuffer = m_device.allocateCommandBuffers(vk::CommandBufferAllocateInfo(m_commandPool, vk::CommandBufferLevel::ePrimary, 1)).front();


    if (!SDL_Vulkan_CreateSurface(sdlWindow, m_instance, reinterpret_cast<VkSurfaceKHR*>(&m_surface))) {
        goto fail_Vulkan_SDLWindow;
    }
}

VK13Fixture::~VK13Fixture() {
    m_device.freeCommandBuffers(m_commandPool, m_commandBuffer);
    m_device.destroyCommandPool(m_commandPool);
    m_device.destroy();
    m_instance.destroy();
}

bool VK13Fixture::fail(Destroy d) {
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