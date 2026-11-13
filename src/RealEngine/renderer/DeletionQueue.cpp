/**
 *  @author    Dubsky Tomas
 */
module realengine:renderer.DeletionQueue_impl;
import :renderer.DeletionQueue;
import :graphics.synchronization.DoubleBuffered;
import :utility.Error;

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
            m_allocator.freeMemory(
                reinterpret_cast<vma::Allocation::NativeType>(front.handle)
            );
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
        m_device.destroy(reinterpret_cast<vk::Semaphore::NativeType>(handle));
        break;
    case vk::ObjectType::eFence:
        m_device.destroy(reinterpret_cast<vk::Fence::NativeType>(handle));
        break;
    case vk::ObjectType::eDeviceMemory:
        m_device.free(reinterpret_cast<vk::DeviceMemory::NativeType>(handle));
        break;
    case vk::ObjectType::eBuffer:
        m_device.destroy(reinterpret_cast<vk::Buffer::NativeType>(handle));
        break;
    case vk::ObjectType::eImage:
        m_device.destroy(reinterpret_cast<vk::Image::NativeType>(handle));
        break;
    case vk::ObjectType::eBufferView:
        m_device.destroy(reinterpret_cast<vk::BufferView::NativeType>(handle));
        break;
    case vk::ObjectType::eImageView:
        m_device.destroy(reinterpret_cast<vk::ImageView::NativeType>(handle));
        break;
    case vk::ObjectType::ePipelineLayout:
        m_device.destroy(reinterpret_cast<vk::PipelineLayout::NativeType>(handle));
        break;
    case vk::ObjectType::eRenderPass:
        m_device.destroy(reinterpret_cast<vk::RenderPass::NativeType>(handle));
        break;
    case vk::ObjectType::ePipeline:
        m_device.destroy(reinterpret_cast<vk::Pipeline::NativeType>(handle));
        break;
    case vk::ObjectType::eDescriptorSetLayout:
        m_device.destroy(reinterpret_cast<vk::DescriptorSetLayout::NativeType>(handle));
        break;
    case vk::ObjectType::eSampler:
        m_device.destroy(reinterpret_cast<vk::Sampler::NativeType>(handle));
        break;
    case vk::ObjectType::eFramebuffer:
        m_device.destroy(reinterpret_cast<vk::Framebuffer::NativeType>(handle));
        break;
    default: error("Unsupported object queued for deletion");
    }
}

} // namespace re
