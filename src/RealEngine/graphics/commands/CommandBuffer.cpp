/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/commands/CommandBuffer.hpp>

namespace re {

CommandBuffer::CommandBuffer(const CommandBufferCreateInfo& createInfo)
    : m_cmdBuf(device()
                   .allocateCommandBuffers(vk::CommandBufferAllocateInfo{
                       commandPool(), createInfo.level, 1u})
                   .back()) {

    setDebugUtilsObjectName(m_cmdBuf, createInfo.debugName);
}

CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept
    : m_cmdBuf(other.m_cmdBuf) {
    other.m_cmdBuf = nullptr;
}

CommandBuffer& CommandBuffer::operator=(CommandBuffer&& other) noexcept {
    std::swap(m_cmdBuf, other.m_cmdBuf);
    return *this;
}

CommandBuffer::~CommandBuffer() {
    device().freeCommandBuffers(commandPool(), m_cmdBuf);
}

void CommandBuffer::submitToGraphicsCompQueue(
    const vk::ArrayProxy<const vk::SubmitInfo2>& submits,
    const vk::Fence&                             signalFence /* = nullptr*/
) {
    graphicsCompQueue().submit2(submits, signalFence);
}

void CommandBuffer::submitToGraphicsCompQueue(const vk::Fence&
                                                  signalFence /* = nullptr*/) const {
    graphicsCompQueue().submit(vk::SubmitInfo{{}, {}, m_cmdBuf}, signalFence);
}

void CommandBuffer::debugBarrier() const {
    auto barrier = re::debugBarrier();
    m_cmdBuf.pipelineBarrier2(vk::DependencyInfo{{}, barrier, {}, {}});
}

} // namespace re