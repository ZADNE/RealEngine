/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vma/vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

#include <RealEngine/renderer/DeletionQueue.hpp>

namespace re {

/**
 * @brief   Provides access to global Vulkan objects (such as device)
 *          to lower-level vulkan objects (such as pipeline)
 */
class VulkanObject {
    friend class VulkanFixture;

protected:
    /**
     * @brief Concrete Vulkan objects (e.g. buffers, textures, pipelines) are
     * supposed to derive from this.
     * @details This constructor is deliberately protected as the VulkanObject
     * does not do anything on its own
     */
    VulkanObject() {}

    static const vk::PhysicalDevice& physicalDevice() {
        return *s_physicalDevice;
    }
    static const vk::Device&     device() { return *s_device; }
    static const vma::Allocator& allocator() { return *s_allocator; }
    static const vk::Queue&      graphicsQueue() { return *s_graphicsQueue; }
    static const vk::Queue&      computeQueue() { return *s_computeQueue; }
    static const vk::Queue& presentationQueue() { return *s_presentationQueue; }
    static const vk::PipelineCache& pipelineCache() { return *s_pipelineCache; }
    static const vk::RenderPass&    renderPass() { return *s_renderPass; }
    static const vk::CommandPool&   commandPool() { return *s_commandPool; }
    static const vk::CommandBuffer& cmdBuf() { return *s_cmdBuf; }
    static const vk::CommandBuffer& oneTimeSubmitCommandBuffer() {
        return *s_oneTimeSubmitCommandBuffer;
    }
    static const vk::DescriptorPool& descriptorPool() {
        return *s_descriptorPool;
    }
    static DeletionQueue& deletionQueue() { return *s_deletionQueue; }

private:
    static inline const vk::PhysicalDevice* s_physicalDevice    = nullptr;
    static inline const vk::Device*         s_device            = nullptr;
    static inline const vma::Allocator*     s_allocator         = nullptr;
    static inline const vk::Queue*          s_graphicsQueue     = nullptr;
    static inline const vk::Queue*          s_computeQueue      = nullptr;
    static inline const vk::Queue*          s_presentationQueue = nullptr;
    static inline const vk::PipelineCache*  s_pipelineCache     = nullptr;
    static inline const vk::RenderPass*     s_renderPass        = nullptr;
    static inline const vk::CommandPool*    s_commandPool       = nullptr;
    static inline const vk::CommandBuffer*  s_cmdBuf            = nullptr;
    static inline const vk::CommandBuffer* s_oneTimeSubmitCommandBuffer = nullptr;
    static inline const vk::DescriptorPool* s_descriptorPool = nullptr;
    static inline DeletionQueue*            s_deletionQueue  = nullptr;
};

} // namespace re