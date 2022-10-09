/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <optional>
#include <memory>

#include <vulkan/vulkan_raii.hpp>

#include <RealEngine/rendering/internal_renderers/VK13Buffer.hpp>
#include <RealEngine/rendering/internal_renderers/VK13Capabilities.hpp>
#include <RealEngine/rendering/internal_renderers/VK13Framebuffer.hpp>
#include <RealEngine/rendering/internal_renderers/VK13Ordering.hpp>
#include <RealEngine/rendering/internal_renderers/VK13ShaderProgram.hpp>
#include <RealEngine/rendering/internal_renderers/VK13Texture.hpp>
#include <RealEngine/rendering/internal_renderers/VK13VertexArray.hpp>
#include <RealEngine/rendering/internal_renderers/VK13Viewport.hpp>

struct SDL_Window;

namespace RE {

/**
* @brief Enforces use of Vulkan 1.3 graphics backend.
*
* This is used internally when the RealEngine starts.
*
* @warning Never use this class directly!
*/
class VK13Fixture {
public:

    /**
    * @brief Tries to prepare for creation of Vulkan 1.3 context.
    * @return True if succeeded.
    * @warning Do not call use() if this fails.
    */
    static bool initialize(SDL_Window* sdlWindow, bool vSync);

    VK13Fixture(SDL_Window* sdlWindow, bool vSync);

private:

    class Implementations {
    public:

        Implementations() {
            assignReferences<RendererLateBind>();
            assignReferences<RendererVK13>();
        }

        ~Implementations() {
            clearReferences<RendererLateBind>();
            clearReferences<RendererVK13>();
        }

        Implementations(const Implementations&) = delete;
        Implementations& operator=(const Implementations&) = delete;

    private:

        template<Renderer R>
        void assignReferences();

        template<Renderer R>
        void clearReferences();

        VK13Buffer m_bufferImpl;
        VK13Capabilities m_capabilitiesImpl;
        VK13Framebuffer m_mainFramebufferImpl;
        VK13Ordering m_orderingImpl;
        VK13ShaderProgram m_shaderProgramImpl;
        VK13Texture m_textureImpl;
        VK13VertexArray m_vertexArrayImpl;
        VK13Viewport m_viewportImpl;

        ViewportState m_viewportState;
    };

    static std::optional<VK13Fixture> s_fixture;

    vk::raii::Context m_context{};
    vk::raii::Instance m_instance;
#ifndef NDEBUG
    vk::raii::DebugUtilsMessengerEXT m_debugUtilsMessenger;
#endif // !NDEBUG
    vk::raii::SurfaceKHR m_surface;
    uint32_t m_graphicsQueueFamilyIndex;
    uint32_t m_presentationQueueFamilyIndex;
    vk::raii::PhysicalDevice m_physicalDevice;
    vk::raii::Device m_device;
    vk::raii::Queue m_graphicsQueue;
    vk::raii::Queue m_presentationQueue;
    vk::raii::SwapchainKHR m_swapchain;
    vk::raii::CommandPool m_commandPool;
    vk::raii::CommandBuffer m_commandBuffer;

    Implementations m_impls;

    vk::raii::Instance createInstance(SDL_Window* sdlWindow);
    vk::raii::DebugUtilsMessengerEXT createDebugUtilsMessenger();
    vk::raii::SurfaceKHR createSurface(SDL_Window* sdlWindow);
    vk::raii::PhysicalDevice createPhysicalDevice();
    vk::raii::Device createDevice();
    vk::raii::Queue createQueue(uint32_t familyIndex);
    vk::raii::SwapchainKHR createSwapchain(SDL_Window* sdlWindow, bool vSync);
    vk::raii::CommandPool createCommandPool();
    vk::raii::CommandBuffer createCommandBuffer();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT sev,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData);
    bool areExtensionsSupported(const vk::raii::PhysicalDevice& physicalDevice);
    bool isSwapchainSupported(const vk::raii::PhysicalDevice& physicalDevice);
    bool findQueueFamilyIndices(const vk::raii::PhysicalDevice& physicalDevice);
};

}