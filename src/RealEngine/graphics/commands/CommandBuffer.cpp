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

void CommandBuffer::submitToGraphicsQueue(
    const vk::ArrayProxy<const vk::SubmitInfo2>& submits,
    const vk::Fence&                             signalFence /* = nullptr*/
) {
    graphicsQueue().submit2(submits, signalFence);
}

void CommandBuffer::submitToComputeQueue(
    const vk::ArrayProxy<const vk::SubmitInfo2>& submits,
    const vk::Fence&                             signalFence /* = nullptr*/
) {
    computeQueue().submit2(submits, signalFence);
}

void CommandBuffer::submitToGraphicsQueue(const vk::Fence& signalFence /* = nullptr*/) const {
    graphicsQueue().submit(vk::SubmitInfo{{}, {}, m_cmdBuf}, signalFence);
}

void CommandBuffer::submitToComputeQueue(const vk::Fence& signalFence /* = nullptr*/) const {
    computeQueue().submit(vk::SubmitInfo{{}, {}, m_cmdBuf}, signalFence);
}

} // namespace re