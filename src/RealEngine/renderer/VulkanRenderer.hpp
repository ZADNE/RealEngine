/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <array>
#include <memory>
#include <vector>

#include <glm/vec4.hpp>
#include <vma/vk_mem_alloc.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <RealEngine/graphics/synchronization/DoubleBuffered.hpp>
#include <RealEngine/graphics/textures/Texture.hpp>
#include <RealEngine/renderer/Allocator.hpp>
#include <RealEngine/rooms/RoomDisplaySettings.hpp>

struct SDL_Window;

namespace re {

/**
 * @brief Constructs chain which requests features needed by the engine.
 * @note  Unfortunatelly this cannot be made constexpr.
 */
const void* defaultDeviceCreateInfoChain();

/**
 * @brief Describes how to initialize Vulkan.
 */
struct VulkanInitInfo {
    struct BufferDescr {
        vk::Format format;
        vk::ImageUsageFlags usage;
        vk::ImageAspectFlags aspects;
    };

    /**
     * @brief Chain of Vulkan structures specifing the features to initialize
     * the device with. If unspecified, only features needed by the engine are
     * initialized.
     */
    const void* deviceCreateInfoChain = defaultDeviceCreateInfoChain();
    /**
     * @brief Additional buffers that should be created.
     * Extent of all of these is the extent of the swapchain.
     */
    std::span<BufferDescr> additionalBuffers;
};

/**
 * @brief   Creates all objects necessary for Vulkan rendering.
 * @details This is used internally when the RealEngine starts.
 * @warning Never use this class directly!
 */
class VulkanRenderer {
public:
    /**
     * @brief Sets up for Vulkan rendering
     * @throws If anything fails
     */
    VulkanRenderer(
        SDL_Window* sdlWindow, bool vSync, std::string_view preferredDevice,
        const VulkanInitInfo& vulkan
    );

    VulkanRenderer(const VulkanRenderer&)            = delete; ///< Noncopyable
    VulkanRenderer& operator=(const VulkanRenderer&) = delete; ///< Noncopyable

    VulkanRenderer(VulkanRenderer&&)            = delete;      ///< Nonmovable
    VulkanRenderer& operator=(VulkanRenderer&&) = delete;      ///< Nonmovable

    ~VulkanRenderer();

    void setMainRenderPass(const RenderPass& rp, uint32_t imGuiSubpassIndex);

    const CommandBuffer& prepareFrame();

    void mainRenderPassBegin(std::span<const vk::ClearValue> clearValues);
    void mainRenderPassNextSubpass();
    void mainRenderPassDrawImGui();
    void mainRenderPassEnd();

    void finishFrame();

    void changePresentation(bool vSync);

    void prepareForDestructionOfRendererObjects();

    /**
     * @brief Lists devices (~GPUs) that can be used for rendering
     */
    std::vector<std::string> availableDevices() const;

    /**
     * @brief Returns name of the device used for rendering
     * @details The device is one of the ones from availableDevices().
     */
    std::string usedDevice() const;

    DeletionQueue& deletionQueue() { return m_deletionQueue; }

private:
    // Vulkan objects
    uint32_t m_imageIndex   = 0u;
    int m_frame             = 0;
    SDL_Window* m_sdlWindow = nullptr;
    vk::raii::Context m_context{};
    vk::raii::Instance m_instance;
#if RE_BUILDING_FOR_DEBUG
    vk::raii::DebugUtilsMessengerEXT m_debugUtilsMessenger;
#endif // RE_BUILDING_FOR_DEBUG
    vk::raii::SurfaceKHR m_surface;
    uint32_t m_graphicsCompQueueFamIndex{};
    uint32_t m_presentationQueueFamIndex{};
    vk::raii::PhysicalDevice m_physicalDevice;
    vk::PresentModeKHR m_presentMode{};
    vk::raii::Device m_device;
    vk::DispatchLoaderDynamic m_dispatchLoaderDynamic{
        *m_instance, vkGetInstanceProcAddr, *m_device, vkGetDeviceProcAddr
    };
    Allocator m_allocator;
    vk::raii::Queue m_graphicsCompQueue;
    vk::raii::Queue m_presentationQueue;
    uint32_t m_minImageCount{};
    vk::Extent2D m_swapchainExtent{};
    vk::raii::SwapchainKHR m_swapchain;
    std::vector<vk::raii::ImageView> m_swapchainImageViews;
    std::vector<VulkanInitInfo::BufferDescr> m_additionalBufferDescrs;
    std::vector<Texture> m_additionalBuffers;
    std::vector<vk::raii::Framebuffer> m_swapChainFramebuffers;
    vk::raii::CommandPool m_commandPool;
    FrameDoubleBuffered<CommandBuffer> m_cbs;
    CommandBuffer m_oneTimeSubmitCmdBuf;
    vk::raii::PipelineCache m_pipelineCache;
    vk::raii::DescriptorPool m_descriptorPool;
    FrameDoubleBuffered<vk::raii::Semaphore> m_imageAvailableSems;
    FrameDoubleBuffered<vk::raii::Semaphore> m_renderingFinishedSems;
    FrameDoubleBuffered<vk::raii::Fence> m_inFlightFences;
    bool m_recreteSwapchain = false;
    DeletionQueue m_deletionQueue{*m_device, m_allocator};

    // Active room dependent
    const RenderPass* m_mainRenderPass{};
    uint32_t m_imGuiSubpassIndex{};

    // Implementations
    void assignImplementationReferences();

    // Create object helpers
    vk::raii::Instance createInstance();
    vk::raii::DebugUtilsMessengerEXT createDebugUtilsMessenger();
    vk::raii::SurfaceKHR createSurface();
    vk::raii::PhysicalDevice createPhysicalDevice(
        std::string_view preferredDevice, const void* deviceCreateInfoChain
    );
    vk::PresentModeKHR selectClosestPresentMode(bool vSync);
    vk::raii::Device createDevice(const void* deviceCreateInfoChain);
    vma::Allocator createAllocator();
    vk::raii::Queue getQueue(uint32_t familyIndex);
    vk::raii::SwapchainKHR createSwapchain();
    std::vector<vk::raii::ImageView> createSwapchainImageViews();
    std::vector<Texture> createAdditionalBuffers();
    std::vector<vk::raii::Framebuffer> createSwapchainFramebuffers();
    vk::raii::CommandPool createCommandPool();
    FrameDoubleBuffered<vk::raii::Semaphore> createSemaphores();
    FrameDoubleBuffered<vk::raii::Fence> createFences();
    vk::raii::PipelineCache createPipelineCache();
    vk::raii::DescriptorPool createDescriptorPool();

    void recreateSwapchain();
};

} // namespace re
