/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vma/vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

#include <RealEngine/renderer/DeletionQueue.hpp>
#include <RealEngine/utility/DebugName.hpp>

namespace re {

class CommandBuffer;

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
    static const vk::Queue& graphicsCompQueue() { return *s_graphicsCompQueue; }
    static const vk::Queue& presentationQueue() { return *s_presentationQueue; }
    static const vk::PipelineCache& pipelineCache() { return *s_pipelineCache; }
    static const vk::RenderPass&    renderPass() { return *s_renderPass; }
    static const vk::CommandPool&   commandPool() { return *s_commandPool; }
    static const CommandBuffer&     cmdBuf() { return *s_cmdBuf; }
    static const CommandBuffer&     oneTimeSubmitCmdBuf() {
        return *s_oneTimeSubmitCmdBuf;
    }
    static const vk::DescriptorPool& descriptorPool() {
        return *s_descriptorPool;
    }
    static const vk::DispatchLoaderDynamic& dispatchLoaderDynamic() {
        return *s_dispatchLoaderDynamic;
    }
    static DeletionQueue& deletionQueue() { return *s_deletionQueue; }

    /**
     * @brief Assign a debug name to a given object, does nothing in release build
     */
    template<typename T>
        requires vk::isVulkanHandleType<T>::value
    static void setDebugUtilsObjectName(T object, DebugName<> debugName) {
        if constexpr (k_buildType == BuildType::Debug) {
            device().setDebugUtilsObjectNameEXT(
                vk::DebugUtilsObjectNameInfoEXT{
                    T::objectType,
                    reinterpret_cast<uint64_t>(static_cast<T::NativeType>(object)),
                    static_cast<const char*>(debugName)},
                dispatchLoaderDynamic()
            );
        }
    }

private:
    static inline const vk::PhysicalDevice* s_physicalDevice      = nullptr;
    static inline const vk::Device*         s_device              = nullptr;
    static inline const vma::Allocator*     s_allocator           = nullptr;
    static inline const vk::Queue*          s_graphicsCompQueue   = nullptr;
    static inline const vk::Queue*          s_presentationQueue   = nullptr;
    static inline const vk::PipelineCache*  s_pipelineCache       = nullptr;
    static inline const vk::RenderPass*     s_renderPass          = nullptr;
    static inline const vk::CommandPool*    s_commandPool         = nullptr;
    static inline const CommandBuffer*      s_cmdBuf              = nullptr;
    static inline const CommandBuffer*      s_oneTimeSubmitCmdBuf = nullptr;
    static inline const vk::DescriptorPool* s_descriptorPool      = nullptr;
    static inline const vk::DispatchLoaderDynamic* s_dispatchLoaderDynamic = nullptr;
    static inline DeletionQueue* s_deletionQueue = nullptr;
};

} // namespace re