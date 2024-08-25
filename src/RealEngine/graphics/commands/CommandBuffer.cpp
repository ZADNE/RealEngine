/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/commands/CommandBuffer.hpp>

namespace re {

CommandBuffer::CommandBuffer(const CommandBufferCreateInfo& createInfo)
    : m_cb(device()
               .allocateCommandBuffers(vk::CommandBufferAllocateInfo{
                   commandPool(), createInfo.level, 1u
               })
               .back()) {

    setDebugUtilsObjectName(m_cb, createInfo.debugName);
}

CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept
    : m_cb(std::exchange(other.m_cb, nullptr)) {
}

CommandBuffer& CommandBuffer::operator=(CommandBuffer&& other) noexcept {
    std::swap(m_cb, other.m_cb);
    return *this;
}

CommandBuffer::~CommandBuffer() {
    device().freeCommandBuffers(commandPool(), m_cb);
}

void CommandBuffer::submitToGraphicsCompQueue(
    const vk::ArrayProxy<const vk::SubmitInfo2>& submits,
    const vk::Fence& signalFence /* = nullptr*/
) {
    graphicsCompQueue().submit2(submits, signalFence);
}

void CommandBuffer::submitToGraphicsCompQueue(const vk::Fence&
                                                  signalFence /* = nullptr*/) const {
    graphicsCompQueue().submit(vk::SubmitInfo{{}, {}, m_cb}, signalFence);
}

void CommandBuffer::debugBarrier() const {
    auto barrier = re::debugBarrier();
    m_cb.pipelineBarrier2(vk::DependencyInfo{{}, barrier, {}, {}});
}

} // namespace re
