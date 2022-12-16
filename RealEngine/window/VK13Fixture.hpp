/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <memory>
#include <array>

#include <vulkan/vulkan_raii.hpp>

#include <glm/vec4.hpp>

#include <RealEngine/rendering/PerFrameInFlight.hpp>

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

    const vk::CommandBuffer& prepareFrame(const glm::vec4& clearColor, bool useImGui);
    void finishFrame(bool useImGui);

    void changePresentation(bool vSync);

    void prepareForDestructionOfRendererObjects();

private:

    //Vulkan objects
    uint32_t m_imageIndex = 0u;
    int m_frame = 0;
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
    vk::raii::CommandBuffer m_oneTimeSubmitCommandBuffer;
    vk::raii::PipelineCache m_pipelineCache;
    vk::raii::DescriptorPool m_descriptorPool;
    PerFrameInFlight<vk::raii::Semaphore> m_imageAvailableSems;
    PerFrameInFlight<vk::raii::Semaphore> m_renderingFinishedSems;
    PerFrameInFlight<vk::raii::Fence> m_inFlightFences;
    bool m_recreteSwapchain = false;
    //Implementations
    void assignImplementationReferences();
    void clearImplementationReferences();
    //Create object helpers
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

    void recreateSwapchain();

    void recreateImGuiFontTexture();
};

}