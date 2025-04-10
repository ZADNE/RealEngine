/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <utility>

#include <vma/vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

#include <RealEngine/renderer/DeletionQueue.hpp>
#include <RealEngine/renderer/PipelineHotLoader.hpp>
#include <RealEngine/utility/DebugString.hpp>

namespace re {

class CommandBuffer;

/**
 * @brief   Provides children Vulkan objects access to global Vulkan objects
 *          (such as device)
 * @details Children Vulkan objects (e.g. buffers, textures, pipelines) derive
 *          from this.
 */
class VulkanObjectBase {
    friend class VulkanFixture;

protected:
    /**
     * @brief This constructor is deliberately protected as the VulkanObjectBase
     * does not do anything on its own
     */
    VulkanObjectBase() {}

    static const vk::PhysicalDevice& physicalDevice() {
        return *s_physicalDevice;
    }
    static const vk::Device& device() { return *s_device; }
    static const vma::Allocator& allocator() { return *s_allocator; }
    static const vk::Queue& graphicsCompQueue() { return *s_graphicsCompQueue; }
    static const vk::Queue& presentationQueue() { return *s_presentationQueue; }
    static const vk::PipelineCache& pipelineCache() { return *s_pipelineCache; }
    static const vk::CommandPool& commandPool() { return *s_commandPool; }
    static const CommandBuffer& oneTimeSubmitCmdBuf() {
        return *s_oneTimeSubmitCmdBuf;
    }
    static const vk::DescriptorPool& descriptorPool() {
        return *s_descriptorPool;
    }
    static const vk::DispatchLoaderDynamic& dispatchLoaderDynamic() {
        return *s_dispatchLoaderDynamic;
    }
    static DeletionQueue& deletionQueue() { return *s_deletionQueue; }
    static PipelineHotLoader& pipelineHotLoader() {
        return *s_pipelineHotLoader;
    }

    /**
     * @brief Assign a debug name to a given object, does nothing in release build
     */
    template<typename T>
        requires vk::isVulkanHandleType<T>::value
    static void setDebugUtilsObjectName(T object, DebugString<> debugName) {
        if constexpr (k_buildType == BuildType::Debug) {
            device().setDebugUtilsObjectNameEXT(
                vk::DebugUtilsObjectNameInfoEXT{
                    T::objectType,
                    reinterpret_cast<uint64_t>(static_cast<T::NativeType>(object)),
                    static_cast<const char*>(debugName)
                },
                dispatchLoaderDynamic()
            );
        }
    }

private:
    static inline const vk::PhysicalDevice* s_physicalDevice = nullptr;
    static inline const vk::Device* s_device                 = nullptr;
    static inline const vma::Allocator* s_allocator          = nullptr;
    static inline const vk::Queue* s_graphicsCompQueue       = nullptr;
    static inline const vk::Queue* s_presentationQueue       = nullptr;
    static inline const vk::PipelineCache* s_pipelineCache   = nullptr;
    static inline const vk::CommandPool* s_commandPool       = nullptr;
    static inline const CommandBuffer* s_oneTimeSubmitCmdBuf = nullptr;
    static inline const vk::DescriptorPool* s_descriptorPool = nullptr;
    static inline const vk::DispatchLoaderDynamic* s_dispatchLoaderDynamic = nullptr;
    static inline DeletionQueue* s_deletionQueue         = nullptr;
    static inline PipelineHotLoader* s_pipelineHotLoader = nullptr;
};

} // namespace re
