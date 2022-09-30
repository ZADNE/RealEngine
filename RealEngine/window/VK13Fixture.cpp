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

VK13Fixture VK13Fixture::s_fixture{};

bool VK13Fixture::initialize(SDL_Window* sdlWindow) {
    return s_fixture.init(sdlWindow);
}

VK13Fixture::VK13Fixture() {
    
}

VK13Fixture::~VK13Fixture() {
    destroy(false);
}

bool VK13Fixture::init(SDL_Window* sdlWindow) {
    using enum VK13Fixture::Initialized;
    try {
        //Get number of required extensions
        unsigned int instanceExtensionCount;
        if (!SDL_Vulkan_GetInstanceExtensions(sdlWindow, &instanceExtensionCount, nullptr)) {
            destroy(true);
        }
        std::vector<const char*> instanceExtensions{instanceExtensionCount};
        if (!SDL_Vulkan_GetInstanceExtensions(sdlWindow, &instanceExtensionCount, instanceExtensions.data())) {
            destroy(true);
        }

        //Create Vulkan instance
        vk::ApplicationInfo applicationInfo("RealEngine", 1, "RealEngine", RE_VERSION, VK_API_VERSION_1_3);
        vk::InstanceCreateInfo instanceCreateInfo({}, &applicationInfo, {}, instanceExtensions);
        m_instance = vk::createInstance(instanceCreateInfo);
        m_init = INSTANCE;

        //Select physical device and its queue
        vk::PhysicalDevice physicalDevice = m_instance.enumeratePhysicalDevices().front();
        std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
        auto propertyIterator = std::find_if(queueFamilyProperties.begin(),
                                                queueFamilyProperties.end(),
                                                [](vk::QueueFamilyProperties const& qfp) { return qfp.queueFlags & vk::QueueFlagBits::eGraphics; });
        size_t graphicsQueueFamilyIndex = std::distance(queueFamilyProperties.begin(), propertyIterator);
        assert(graphicsQueueFamilyIndex < queueFamilyProperties.size());

        //Create device
        float deviceQueuePriority = 1.0f;
        vk::DeviceQueueCreateInfo deviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), static_cast<uint32_t>(graphicsQueueFamilyIndex), 1, &deviceQueuePriority);
        m_device = physicalDevice.createDevice(vk::DeviceCreateInfo(vk::DeviceCreateFlags(), deviceQueueCreateInfo));
        m_init = DEVICE;

        //Create commnad pool
        m_commandPool = m_device.createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlags(), graphicsQueueFamilyIndex));
        m_init = COMMAND_POOL;

        //Create commnad buffer
        m_commandBuffer = m_device.allocateCommandBuffers(vk::CommandBufferAllocateInfo(m_commandPool, vk::CommandBufferLevel::ePrimary, 1)).front();
        m_init = COMMAND_BUFFER;

        if (!SDL_Vulkan_CreateSurface(sdlWindow, m_instance, reinterpret_cast<VkSurfaceKHR*>(&m_surface))) {
            destroy(true);
        }
    }
    catch (std::exception& e) {
        destroy(false);
        return false;
    }
    return true;
}

void VK13Fixture::destroy(bool throwException) {
    using enum VK13Fixture::Initialized;
    switch (m_init) {
    case COMMAND_BUFFER:
        m_device.freeCommandBuffers(m_commandPool, m_commandBuffer);
    case COMMAND_POOL:
        m_device.destroyCommandPool(m_commandPool);
    case DEVICE:
        m_device.destroy();
    case INSTANCE:
        m_instance.destroy();
    case NOTHING:
        m_init = NOTHING;
    }
    if (throwException) {
        throw std::exception("Vulkan initialization failed");
    }
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