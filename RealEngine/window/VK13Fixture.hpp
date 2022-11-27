/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <memory>
#include <array>

#include <vulkan/vulkan_raii.hpp>

#include <RealEngine/rendering/internal_renderers/VK13Buffer.hpp>
#include <RealEngine/rendering/internal_renderers/VK13Capabilities.hpp>
#include <RealEngine/rendering/internal_renderers/VK13Framebuffer.hpp>
#include <RealEngine/rendering/internal_renderers/VK13Ordering.hpp>
#include <RealEngine/rendering/internal_renderers/VK13Pipeline.hpp>
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
     * @brief Sets up for Vulkan 1.3 rendering
     * @throws If anything fails
    */
    VK13Fixture(SDL_Window* sdlWindow, bool vSync);

    VK13Fixture(const VK13Fixture&) = delete;
    VK13Fixture& operator=(const VK13Fixture&) = delete;

    VK13Fixture(VK13Fixture&&) = default;
    VK13Fixture& operator=(VK13Fixture&&) = default;

    ~VK13Fixture();

    void prepareFrame(const glm::vec4& clearColor, bool useImGui);
    void finishFrame(bool useImGui);

    void changePresentation(bool vSync);

private:

    void recreateSwapchain();

    class Implementations {
    public:

        Implementations(const vk::Device& device, const vk::PipelineCache& pipelineCache, const vk::RenderPass& renderPass) :
            m_pipelineImpl(device, pipelineCache, renderPass) {
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
        VK13Pipeline m_pipelineImpl;
        VK13ShaderProgram m_shaderProgramImpl;
        VK13Texture m_textureImpl;
        VK13VertexArray m_vertexArrayImpl;
        VK13Viewport m_viewportImpl;

        ViewportState m_viewportState;
    };

    static constexpr size_t FRAMES_IN_FLIGHT = 2;
    template<typename T>
    using PerFrameInFlight = std::array<T, FRAMES_IN_FLIGHT>;


    uint32_t m_imageIndex = 0u;
    size_t m_frame = 0;
    SDL_Window* m_sdlWindow = nullptr;
    bool m_vSync = false;
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
    uint32_t m_minImageCount;
    vk::Extent2D m_swapchainExtent;
    vk::raii::SwapchainKHR m_swapchain;
    std::vector<vk::raii::ImageView> m_swapchainImageViews;
    vk::raii::RenderPass m_renderPass;
    std::vector<vk::raii::Framebuffer> m_swapChainFramebuffers;
    vk::raii::CommandPool m_commandPool;
    PerFrameInFlight<vk::raii::CommandBuffer> m_commandBuffers;
    vk::raii::PipelineCache m_pipelineCache;
    vk::raii::DescriptorPool m_descriptorPool;
    PerFrameInFlight<vk::raii::Semaphore> m_imageAvailableSems;
    PerFrameInFlight<vk::raii::Semaphore> m_renderingFinishedSems;
    PerFrameInFlight<vk::raii::Fence> m_inFlightFences;
    bool m_recreteSwapchain = false;

    Implementations m_impls{*m_device, *m_pipelineCache, *m_renderPass};

    vk::raii::Instance createInstance();
    vk::raii::DebugUtilsMessengerEXT createDebugUtilsMessenger();
    vk::raii::SurfaceKHR createSurface();
    vk::raii::PhysicalDevice createPhysicalDevice();
    vk::raii::Device createDevice();
    vk::raii::Queue createQueue(uint32_t familyIndex);
    vk::raii::SwapchainKHR createSwapchain();
    std::vector<vk::raii::ImageView> createSwapchainImageViews();
    vk::raii::RenderPass createRenderPass();
    std::vector<vk::raii::Framebuffer> createSwapchainFramebuffers();
    vk::raii::CommandPool createCommandPool();
    PerFrameInFlight<vk::raii::CommandBuffer> createCommandBuffers();
    PerFrameInFlight<vk::raii::Semaphore> createSemaphores();
    PerFrameInFlight<vk::raii::Fence> createFences();
    vk::raii::PipelineCache createPipelineCache();
    vk::raii::DescriptorPool createDescriptorPool();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT sev,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData);
    bool areExtensionsSupported(const vk::raii::PhysicalDevice& physicalDevice);
    bool isSwapchainSupported(const vk::raii::PhysicalDevice& physicalDevice);
    bool findQueueFamilyIndices(const vk::raii::PhysicalDevice& physicalDevice);

    void recreateImGuiFontTexture();
};

}