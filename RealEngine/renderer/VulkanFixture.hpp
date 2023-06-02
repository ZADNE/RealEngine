/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <array>
#include <memory>

#include <glm/vec4.hpp>
#include <vma/vk_mem_alloc.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <RealEngine/renderer/Allocator.hpp>
#include <RealEngine/renderer/VulkanObject.hpp>
#include <RealEngine/graphics/PerFrameInFlight.hpp>

struct SDL_Window;

namespace re {

/**
 * @brief Enforces use of Vulkan graphics backend.
 *
 * This is used internally when the RealEngine starts.
 *
 * @warning Never use this class directly!
 */
class VulkanFixture {
public:
    /**
     * @brief Sets up for Vulkan rendering
     * @throws If anything fails
     */
    VulkanFixture(SDL_Window* sdlWindow, bool vSync);

    VulkanFixture(const VulkanFixture&)            = delete; /**< Noncopyable */
    VulkanFixture& operator=(const VulkanFixture&) = delete; /**< Noncopyable */

    VulkanFixture(VulkanFixture&&)            = delete; /**< Nonmovable */
    VulkanFixture& operator=(VulkanFixture&&) = delete; /**< Nonmovable */

    ~VulkanFixture();

    const vk::CommandBuffer& prepareFrame(
        const glm::vec4& clearColor, bool useImGui
    );
    void finishFrame(bool useImGui);

    void changePresentation(bool vSync);

    void prepareForDestructionOfRendererObjects();

private:
    // Vulkan objects
    uint32_t           m_imageIndex = 0u;
    int                m_frame      = 0;
    SDL_Window*        m_sdlWindow  = nullptr;
    bool               m_vSync      = false;
    vk::raii::Context  m_context{};
    vk::raii::Instance m_instance;
#ifndef NDEBUG
    vk::raii::DebugUtilsMessengerEXT m_debugUtilsMessenger;
#endif // !NDEBUG
    vk::raii::SurfaceKHR                      m_surface;
    uint32_t                                  m_graphicsQueueFamilyIndex;
    uint32_t                                  m_computeQueueFamilyIndex;
    uint32_t                                  m_presentationQueueFamilyIndex;
    vk::raii::PhysicalDevice                  m_physicalDevice;
    vk::raii::Device                          m_device;
    Allocator                                 m_allocator;
    vk::raii::Queue                           m_graphicsQueue;
    vk::raii::Queue                           m_computeQueue;
    vk::raii::Queue                           m_presentationQueue;
    uint32_t                                  m_minImageCount;
    vk::Extent2D                              m_swapchainExtent;
    vk::raii::SwapchainKHR                    m_swapchain;
    std::vector<vk::raii::ImageView>          m_swapchainImageViews;
    vk::raii::RenderPass                      m_renderPass;
    std::vector<vk::raii::Framebuffer>        m_swapChainFramebuffers;
    vk::raii::CommandPool                     m_commandPool;
    PerFrameInFlight<vk::raii::CommandBuffer> m_commandBuffers;
    vk::raii::CommandBuffer                   m_oneTimeSubmitCommandBuffer;
    vk::raii::PipelineCache                   m_pipelineCache;
    vk::raii::DescriptorPool                  m_descriptorPool;
    PerFrameInFlight<vk::raii::Semaphore>     m_imageAvailableSems;
    PerFrameInFlight<vk::raii::Semaphore>     m_renderingFinishedSems;
    PerFrameInFlight<vk::raii::Fence>         m_inFlightFences;
    bool                                      m_recreteSwapchain = false;
    DeletionQueue m_deletionQueue{*m_device, m_allocator};
    // Implementations
    void assignImplementationReferences();
    void clearImplementationReferences();
    // Create object helpers
    vk::raii::Instance                        createInstance();
    vk::raii::DebugUtilsMessengerEXT          createDebugUtilsMessenger();
    vk::raii::SurfaceKHR                      createSurface();
    vk::raii::PhysicalDevice                  createPhysicalDevice();
    vk::raii::Device                          createDevice();
    vma::Allocator                            createAllocator();
    vk::raii::Queue                           createQueue(uint32_t familyIndex);
    vk::raii::SwapchainKHR                    createSwapchain();
    std::vector<vk::raii::ImageView>          createSwapchainImageViews();
    vk::raii::RenderPass                      createRenderPass();
    std::vector<vk::raii::Framebuffer>        createSwapchainFramebuffers();
    vk::raii::CommandPool                     createCommandPool();
    PerFrameInFlight<vk::raii::CommandBuffer> createCommandBuffers();
    PerFrameInFlight<vk::raii::Semaphore>     createSemaphores();
    PerFrameInFlight<vk::raii::Fence>         createFences();
    vk::raii::PipelineCache                   createPipelineCache();
    vk::raii::DescriptorPool                  createDescriptorPool();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT      sev,
        VkDebugUtilsMessageTypeFlagsEXT             type,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void*                                       userData
    );
    bool areExtensionsSupported(const vk::raii::PhysicalDevice& physicalDevice);
    bool isSwapchainSupported(const vk::raii::PhysicalDevice& physicalDevice);
    bool findQueueFamilyIndices(const vk::raii::PhysicalDevice& physicalDevice);

    void recreateSwapchain();

    void recreateImGuiFontTexture();
};

} // namespace re