/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/CommandBuffer.hpp>

namespace re {

CommandBuffer::CommandBuffer(vk::CommandBufferLevel level)
    : m_commandBuffer(device()
                          .allocateCommandBuffers(vk::CommandBufferAllocateInfo{
                              commandPool(), level, 1u})
                          .back()) {
}

CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept
    : m_commandBuffer(other.m_commandBuffer) {
    other.m_commandBuffer = nullptr;
}

CommandBuffer& CommandBuffer::operator=(CommandBuffer&& other) noexcept {
    std::swap(m_commandBuffer, other.m_commandBuffer);
    return *this;
}

CommandBuffer::~CommandBuffer() {
    device().freeCommandBuffers(commandPool(), m_commandBuffer);
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

void CommandBuffer::submitToGraphicsQueue(const vk::Fence&
                                              signalFence /* = nullptr*/)
    const {
    graphicsQueue().submit(
        vk::SubmitInfo{{}, {}, m_commandBuffer}, signalFence
    );
}

void CommandBuffer::submitToComputeQueue(const vk::Fence&
                                             signalFence /* = nullptr*/) const {
    computeQueue().submit(vk::SubmitInfo{{}, {}, m_commandBuffer}, signalFence);
}

} // namespace re