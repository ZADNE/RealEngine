/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <queue>
#include <type_traits>

#include <vma/vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

namespace re {

/**
 * @brief   Allows delayed deletion of Vulkan and Vma objects
 * @detail  Objects in the queue are batched into groups.
 *          Object are deleted in the order they were added in.
 */
class DeletionQueue {
    friend class VulkanFixture;

public:
    DeletionQueue(const vk::Device& device, const vma::Allocator& allocator);

    DeletionQueue(const DeletionQueue&)            = delete; /**< Noncopyable */
    DeletionQueue& operator=(const DeletionQueue&) = delete; /**< Noncopyable */

    DeletionQueue(DeletionQueue&&)            = delete; /**< Nonmovable */
    DeletionQueue& operator=(DeletionQueue&&) = delete; /**< Nonmovable */

    ~DeletionQueue();

    /**
     * @brief   Begins new group
     */
    void beginNewGroup();

    /**
     * @brief   Deletes next group (that is the oldest group that was not
     * deleted yet)
     * @return  True if the queue was not empty and the group was deleted, false
     * otherwise
     */
    bool deleteNextGroup();

    /**
     * @brief Enqueues deletion of a Vulkan object
     */
    template<typename T>
        requires vk::isVulkanHandleType<T>::value
    void enqueueDeletion(const T& object) {
        if (object) {
            m_queue.emplace(
                QueueRecord::Category::VulkanHandle,
                T::objectType,
                static_cast<T::NativeType>(object)
            );
        }
    }

    /**
     * @brief Enqueues freeing of a Vma allocation
     */
    void enqueueDeletion(const vma::Allocation& allocation) {
        if (allocation) {
            m_queue.emplace(
                QueueRecord::Category::VmaAllocation,
                vk::ObjectType::eUnknown,
                static_cast<VmaAllocation>(allocation)
            );
        }
    }

private:
    struct QueueRecord {

        enum class Category { Separator, VulkanHandle, VmaAllocation };

        QueueRecord(Category _cat, vk::ObjectType _type, void* _handle)
            : cat(_cat)
            , type(_type)
            , handle(_handle) {}

        Category       cat;
        vk::ObjectType type;   /**< Determines type of the handle */
        void*          handle; /**< Is the handle to the object to be deleted */
        static_assert(VK_USE_64_BIT_PTR_DEFINES == 1);
    };

    void deleteVulkanHandle(vk::ObjectType type, void* handle);

    std::queue<QueueRecord> m_queue;
    const vk::Device&       m_device;
    const vma::Allocator&   m_allocator;
};

} // namespace re