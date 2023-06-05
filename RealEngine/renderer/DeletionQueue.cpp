/*!
 *  @author    Dubsky Tomas
 */
#include <any>

#include <RealEngine/renderer/DeletionQueue.hpp>
#include <RealEngine/utility/Error.hpp>

namespace re {

DeletionQueue::DeletionQueue(const vk::Device& device, const vma::Allocator& allocator)
    : m_device(device)
    , m_allocator(allocator) {
}

DeletionQueue::~DeletionQueue() {
    while (deleteNextGroup()) {
        // Delete all groups in the correct order
    }
}

void DeletionQueue::beginNewGroup() {
    m_queue.emplace(QueueRecord::Category::Separator, vk::ObjectType::eUnknown, nullptr);
}

bool DeletionQueue::deleteNextGroup() {
    bool groupDeleted = false;
    while (!m_queue.empty() && !groupDeleted) {
        const QueueRecord& front = m_queue.front();
        switch (front.cat) {
        case QueueRecord::Category::Separator: groupDeleted = true; break;
        case QueueRecord::Category::VulkanHandle:
            deleteVulkanHandle(front.type, front.handle);
            break;
        case QueueRecord::Category::VmaAllocation:
            m_allocator.freeMemory(reinterpret_cast<VmaAllocation>(front.handle));
            break;
        default: break;
        }

        m_queue.pop();
    }
    return groupDeleted;
}

void DeletionQueue::deleteVulkanHandle(vk::ObjectType type, void* handle) {
    switch (type) {
    case vk::ObjectType::eBuffer:
        m_device.destroy(reinterpret_cast<VkBuffer>(handle));
        break;
    case vk::ObjectType::eBufferView:
        m_device.destroy(reinterpret_cast<VkBufferView>(handle));
        break;
    case vk::ObjectType::eImage:
        m_device.destroy(reinterpret_cast<VkImage>(handle));
        break;
    case vk::ObjectType::eImageView:
        m_device.destroy(reinterpret_cast<VkImageView>(handle));
        break;
    case vk::ObjectType::eSampler:
        m_device.destroy(reinterpret_cast<VkSampler>(handle));
        break;
    case vk::ObjectType::eDeviceMemory:
        m_device.free(reinterpret_cast<VkDeviceMemory>(handle));
        break;
    case vk::ObjectType::eDescriptorSetLayout:
        m_device.destroy(reinterpret_cast<VkDescriptorSetLayout>(handle));
        break;
    case vk::ObjectType::ePipeline:
        m_device.destroy(reinterpret_cast<VkPipeline>(handle));
        break;
    case vk::ObjectType::ePipelineLayout:
        m_device.destroy(reinterpret_cast<VkPipelineLayout>(handle));
        break;
    case vk::ObjectType::eFence:
        m_device.destroy(reinterpret_cast<VkFence>(handle));
        break;
    case vk::ObjectType::eSemaphore:
        m_device.destroy(reinterpret_cast<VkSemaphore>(handle));
        break;
    default: error("Unsupported object queued for deletion");
    }
}

} // namespace re