/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <queue>

#include <vulkan/vulkan.hpp>


namespace RE {

/**
 * @brief   Allows delayed deletion of Vulkan objects
 * @detail  Objects in the queue are batched into groups.
 *          Object are deleted in the order they were added in.
*/
class VulkanDeletionQueue {
    friend class VulkanFixture;
public:

    VulkanDeletionQueue(const vk::Device& device): m_device(device) {}

    VulkanDeletionQueue(const VulkanDeletionQueue&) = delete;           /**< Noncopyable */
    VulkanDeletionQueue& operator=(const VulkanDeletionQueue&) = delete;/**< Noncopyable */

    VulkanDeletionQueue(VulkanDeletionQueue&&) = delete;                /**< Nonmovable */
    VulkanDeletionQueue& operator=(VulkanDeletionQueue&&) = delete;     /**< Nonmovable */

    ~VulkanDeletionQueue();


    /**
     * @brief   Begins new group
    */
    void beginNewGroup();

    /**
     * @brief   Deletes next group (that is the oldest group that was not deleted yet)
     * @return  True if the queue was not empty and the group was deleted, false otherwise
    */
    bool deleteNextGroup();

    /**
     * @brief Enqueues deletion of a Vulkan object
    */
    template<typename VulkanHandleType> requires vk::isVulkanHandleType<VulkanHandleType>::value
    void enqueueDeletion(VulkanHandleType object) {
        if (object) {
            m_queue.emplace(VulkanHandleType::objectType, VulkanHandleType::NativeType(object));
        }
    }

private:

    struct DeletionRecord {
        DeletionRecord(vk::ObjectType _type, void* _handle): type(_type), handle(_handle) {}

        vk::ObjectType type;    /**< Determines type of the handle, eUnknown separates groups */
        void* handle;           /**< Is the handle to the object to be deleted */
        static_assert(VK_USE_64_BIT_PTR_DEFINES == 1);
    };

    std::queue<DeletionRecord> m_queue;
    const vk::Device& m_device;
};

}