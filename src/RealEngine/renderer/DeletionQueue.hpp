/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <queue>
#include <type_traits>

#include <vma/vk_mem_alloc.hpp>
#include <vulkan/vulkan.hpp>

namespace re {

/**
 * @brief   Allows delayed deletion of Vulkan and VMA objects
 * @details There are two queues: one for objects that are used in simulation
 *          steps and one for objects that are used frame rendering.
 */
class DeletionQueue {
public:
    DeletionQueue(const vk::Device& device, const vma::Allocator& allocator);

    DeletionQueue(const DeletionQueue&)            = delete; ///< Noncopyable
    DeletionQueue& operator=(const DeletionQueue&) = delete; ///< Noncopyable

    DeletionQueue(DeletionQueue&&)            = delete;      ///< Nonmovable
    DeletionQueue& operator=(DeletionQueue&&) = delete;      ///< Nonmovable

    ~DeletionQueue();

    enum class Timeline {
        Step,
        Render
    };

    /**
     * @brief   Deletes all objects from the iteration before previous and
     *          starts new iteration of the timeline.
     * @details Subsequent deletions will be enqueued to the provided timeline,
     *          until this function is called again.
     */
    void startNextIteration(Timeline timeline);

    /**
     * @brief Enqueues deletion of a Vulkan object
     */
    template<typename T>
        requires vk::isVulkanHandleType<T>::value
    void enqueueDeletion(const T& object) {
        if (object) {
            m_queues[std::to_underlying(m_currentTimeline)].emplace(
                QueueRecord::Category::VulkanHandle, T::objectType,
                static_cast<T::NativeType>(object)
            );
        }
    }

    /**
     * @brief Enqueues freeing of a VMA allocation
     */
    void enqueueDeletion(const vma::Allocation& allocation) {
        if (allocation) {
            m_queues[std::to_underlying(m_currentTimeline)].emplace(
                QueueRecord::Category::VmaAllocation, vk::ObjectType::eUnknown,
                static_cast<VmaAllocation>(allocation)
            );
        }
    }

private:
    struct QueueRecord {

        enum class Category {
            Separator,
            VulkanHandle,
            VmaAllocation
        };

        static QueueRecord separator() {
            return QueueRecord{
                QueueRecord::Category::Separator, vk::ObjectType::eUnknown, nullptr
            };
        }

        QueueRecord(Category _cat, vk::ObjectType _type, void* _handle)
            : cat(_cat)
            , type(_type)
            , handle(_handle) {}

        Category cat;
        vk::ObjectType type; ///< Determines type of the handle
        void* handle;        ///< Is the handle to the object to be deleted
        static_assert(VK_USE_64_BIT_PTR_DEFINES == 1);
    };

    bool deleteIteration(std::queue<QueueRecord>& queue);

    void deleteVulkanHandle(vk::ObjectType type, void* handle);

    Timeline m_currentTimeline{};
    std::array<std::queue<QueueRecord>, 2> m_queues{};
    const vk::Device& m_device;
    const vma::Allocator& m_allocator;
};

} // namespace re
