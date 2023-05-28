/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/renderer/VulkanDeletionQueue.hpp>

#include <RealEngine/utility/Error.hpp>


namespace RE {

VulkanDeletionQueue::~VulkanDeletionQueue() {
    while (deleteNextGroup()) {
        //Delete all groups in the correct order
    }
}

void VulkanDeletionQueue::beginNewGroup() {
    m_queue.emplace(vk::ObjectType::eUnknown, nullptr);
}

bool VulkanDeletionQueue::deleteNextGroup() {
    bool groupDeleted = false;
    while (!m_queue.empty() && !groupDeleted) {
        const DeletionRecord& front = m_queue.front();
        switch (front.type) {
        case vk::ObjectType::eUnknown:
            groupDeleted = true; break;
        case vk::ObjectType::eBuffer:
            m_device.destroyBuffer(reinterpret_cast<VkBuffer>(front.handle)); break;
        case vk::ObjectType::eBufferView:
            m_device.destroyBufferView(reinterpret_cast<VkBufferView>(front.handle)); break;
        case vk::ObjectType::eImage:
            m_device.destroyImage(reinterpret_cast<VkImage>(front.handle)); break;
        case vk::ObjectType::eImageView:
            m_device.destroyImageView(reinterpret_cast<VkImageView>(front.handle)); break;
        case vk::ObjectType::eSampler:
            m_device.destroySampler(reinterpret_cast<VkSampler>(front.handle)); break;
        case vk::ObjectType::eDeviceMemory:
            m_device.freeMemory(reinterpret_cast<VkDeviceMemory>(front.handle)); break;
        case vk::ObjectType::eDescriptorSetLayout:
            m_device.destroyDescriptorSetLayout(reinterpret_cast<VkDescriptorSetLayout>(front.handle)); break;
        case vk::ObjectType::ePipeline:
            m_device.destroyPipeline(reinterpret_cast<VkPipeline>(front.handle)); break;
        case vk::ObjectType::ePipelineLayout:
            m_device.destroyPipelineLayout(reinterpret_cast<VkPipelineLayout>(front.handle)); break;
        case vk::ObjectType::eFence:
            m_device.destroyFence(reinterpret_cast<VkFence>(front.handle)); break;
        case vk::ObjectType::eSemaphore:
            m_device.destroy(reinterpret_cast<VkSemaphore>(front.handle)); break;
        default: error("Unsupported object queued for deletion");
        }
        m_queue.pop();
    }
    return groupDeleted;
}

}