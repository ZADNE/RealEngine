/**
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/synchronization/DoubleBuffered.hpp>
#include <RealEngine/renderer/DeletionQueue.hpp>
#include <RealEngine/utility/Error.hpp>

namespace re {

DeletionQueue::DeletionQueue(const vk::Device& device, const vma::Allocator& allocator)
    : m_device(device)
    , m_allocator(allocator) {
    // Insert initial separators to delay deletion
    for (auto& queue : m_queues) {
        for (size_t i = 0; i < k_maxFramesInFlight; i++) {
            queue.emplace(QueueRecord::separator());
        }
    }
}

DeletionQueue::~DeletionQueue() {
    // Delete objects in all queues in the correct order
    for (auto& queue : m_queues) {
        while (deleteIteration(queue)) {
            // Delete all remaining iterations
        }
    }
}

void DeletionQueue::startNextIteration(Timeline timeline) {
    m_currentTimeline = timeline;
    auto& queue       = m_queues[std::to_underlying(timeline)];
    queue.emplace(QueueRecord::separator());
    deleteIteration(queue);
}

bool DeletionQueue::deleteIteration(std::queue<QueueRecord>& queue) {
    bool iterationDeleted = false;
    while (!queue.empty() && !iterationDeleted) {
        const QueueRecord& front = queue.front();
        switch (front.cat) {
        case QueueRecord::Category::Separator: iterationDeleted = true; break;
        case QueueRecord::Category::VulkanHandle:
            deleteVulkanHandle(front.type, front.handle);
            break;
        case QueueRecord::Category::VmaAllocation:
            m_allocator.freeMemory(reinterpret_cast<VmaAllocation>(front.handle));
            break;
        default: break;
        }

        queue.pop();
    }
    return iterationDeleted;
}

void DeletionQueue::deleteVulkanHandle(vk::ObjectType type, void* handle) {
    switch (type) {
    case vk::ObjectType::eSemaphore:
        m_device.destroy(reinterpret_cast<VkSemaphore>(handle));
        break;
    case vk::ObjectType::eFence:
        m_device.destroy(reinterpret_cast<VkFence>(handle));
        break;
    case vk::ObjectType::eDeviceMemory:
        m_device.free(reinterpret_cast<VkDeviceMemory>(handle));
        break;
    case vk::ObjectType::eBuffer:
        m_device.destroy(reinterpret_cast<VkBuffer>(handle));
        break;
    case vk::ObjectType::eImage:
        m_device.destroy(reinterpret_cast<VkImage>(handle));
        break;
    case vk::ObjectType::eBufferView:
        m_device.destroy(reinterpret_cast<VkBufferView>(handle));
        break;
    case vk::ObjectType::eImageView:
        m_device.destroy(reinterpret_cast<VkImageView>(handle));
        break;
    case vk::ObjectType::ePipelineLayout:
        m_device.destroy(reinterpret_cast<VkPipelineLayout>(handle));
        break;
    case vk::ObjectType::eRenderPass:
        m_device.destroy(reinterpret_cast<VkRenderPass>(handle));
        break;
    case vk::ObjectType::ePipeline:
        m_device.destroy(reinterpret_cast<VkPipeline>(handle));
        break;
    case vk::ObjectType::eDescriptorSetLayout:
        m_device.destroy(reinterpret_cast<VkDescriptorSetLayout>(handle));
        break;
    case vk::ObjectType::eSampler:
        m_device.destroy(reinterpret_cast<VkSampler>(handle));
        break;
    case vk::ObjectType::eFramebuffer:
        m_device.destroy(reinterpret_cast<VkFramebuffer>(handle));
        break;
    default: error("Unsupported object queued for deletion");
    }
}

} // namespace re
