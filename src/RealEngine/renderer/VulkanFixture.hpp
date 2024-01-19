/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <array>
#include <memory>

#include <glm/vec4.hpp>
#include <vma/vk_mem_alloc.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <RealEngine/graphics/synchronization/DoubleBuffered.hpp>
#include <RealEngine/graphics/textures/Texture.hpp>
#include <RealEngine/renderer/Allocator.hpp>
#include <RealEngine/renderer/VulkanObject.hpp>

struct SDL_Window;

namespace re {

/**
 * @brief Surface format that is used for default surface
 */
constexpr vk::SurfaceFormatKHR k_surfaceFormat{
    vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};

/**
 * @brief Describes how to initialize Vulkan.
 */
struct VulkanInitInfo {
    struct BufferDescr {
        vk::Format           format;
        vk::ImageUsageFlags  usage;
        vk::ImageAspectFlags aspects;
    };

    /**
     * @brief Chain of Vulkan structures specifing the
     * features to initialize the device with. If unspecified, only features
     * needed by the engine are initialized.
     */
    const void* deviceCreateInfoChain = nullptr;
    /**
     * @brief Is used to create custom main RenderPass. A default
     * single-subpass Renderpass is created instead if this is nullptr.
     */
    vk::RenderPassCreateInfo2* mainRenderPass = nullptr;
    /**
     * @brief Index of subpass within main RenderPass in which ImGui will be
     * rendered
     */
    uint32_t imGuiSubpassIndex = 0;
    /**
     * @brief Additional buffers that should be created.
     * Extent of all of these is the extent of the swapchain.
     */
    std::span<BufferDescr> additionalBuffers;
};

/**
 * @brief Enforces use of Vulkan graphics backend.
 * @details This is used internally when the RealEngine starts.
 * @warning Never use this class directly!
 */
class VulkanFixture {
public:
    /**
     * @brief Sets up for Vulkan rendering
     * @throws If anything fails
     */
    VulkanFixture(SDL_Window* sdlWindow, bool vSync, const VulkanInitInfo& initInfo);

    VulkanFixture(const VulkanFixture&)            = delete; /**< Noncopyable */
    VulkanFixture& operator=(const VulkanFixture&) = delete; /**< Noncopyable */

    VulkanFixture(VulkanFixture&&)            = delete; /**< Nonmovable */
    VulkanFixture& operator=(VulkanFixture&&) = delete; /**< Nonmovable */

    ~VulkanFixture();

    const re::CommandBuffer& prepareFrame(bool useImGui);

    void mainRenderPassBegin(std::span<const vk::ClearValue> clearValues);
    void mainRenderPassNextSubpass();
    void mainRenderPassDrawImGui();
    void mainRenderPassEnd();

    void finishFrame();

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
    vk::raii::SurfaceKHR                     m_surface;
    uint32_t                                 m_graphicsQueueFamilyIndex;
    uint32_t                                 m_computeQueueFamilyIndex;
    uint32_t                                 m_presentationQueueFamilyIndex;
    vk::raii::PhysicalDevice                 m_physicalDevice;
    vk::raii::Device                         m_device;
    Allocator                                m_allocator;
    vk::raii::Queue                          m_graphicsQueue;
    vk::raii::Queue                          m_computeQueue;
    vk::raii::Queue                          m_presentationQueue;
    uint32_t                                 m_minImageCount;
    vk::Extent2D                             m_swapchainExtent;
    vk::raii::SwapchainKHR                   m_swapchain;
    std::vector<vk::raii::ImageView>         m_swapchainImageViews;
    std::vector<VulkanInitInfo::BufferDescr> m_additionalBufferDescrs;
    std::vector<Texture>                     m_additionalBuffers;
    vk::raii::RenderPass                     m_renderPass;
    std::vector<vk::raii::Framebuffer>       m_swapChainFramebuffers;
    vk::raii::CommandPool                    m_commandPool;
    FrameDoubleBuffered<CommandBuffer>       m_cmdBufs;
    CommandBuffer                            m_oneTimeSubmitCmdBuf;
    vk::raii::PipelineCache                  m_pipelineCache;
    vk::raii::DescriptorPool                 m_descriptorPool;
    FrameDoubleBuffered<vk::raii::Semaphore> m_imageAvailableSems;
    FrameDoubleBuffered<vk::raii::Semaphore> m_renderingFinishedSems;
    FrameDoubleBuffered<vk::raii::Fence>     m_inFlightFences;
    bool                                     m_recreteSwapchain = false;
    vk::DispatchLoaderDynamic                m_dispatchLoaderDynamic{
        *m_instance, vkGetInstanceProcAddr, *m_device, vkGetDeviceProcAddr};
    DeletionQueue m_deletionQueue{*m_device, m_allocator};

    // Implementations
    void assignImplementationReferences();

    // Create object helpers
    vk::raii::Instance               createInstance();
    vk::raii::DebugUtilsMessengerEXT createDebugUtilsMessenger();
    vk::raii::SurfaceKHR             createSurface();
    vk::raii::PhysicalDevice         createPhysicalDevice();
    vk::raii::Device       createDevice(const void* deviceCreateInfoChain);
    vma::Allocator         createAllocator();
    vk::raii::Queue        createQueue(uint32_t familyIndex);
    vk::raii::SwapchainKHR createSwapchain();
    std::vector<vk::raii::ImageView>         createSwapchainImageViews();
    std::vector<Texture>                     createAdditionalBuffers();
    vk::raii::RenderPass                     createRenderPass();
    std::vector<vk::raii::Framebuffer>       createSwapchainFramebuffers();
    vk::raii::CommandPool                    createCommandPool();
    FrameDoubleBuffered<vk::raii::Semaphore> createSemaphores();
    FrameDoubleBuffered<vk::raii::Fence>     createFences();
    vk::raii::PipelineCache                  createPipelineCache();
    vk::raii::DescriptorPool                 createDescriptorPool();

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
