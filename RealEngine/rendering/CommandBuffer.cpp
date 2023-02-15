/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/CommandBuffer.hpp>


namespace RE {

CommandBuffer::CommandBuffer(vk::CommandBufferLevel level):
    m_commandBuffer(device().allocateCommandBuffers(vk::CommandBufferAllocateInfo{commandPool(), level, 1u}).back()) {
}

CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept:
    m_commandBuffer(other.m_commandBuffer) {
    other.m_commandBuffer = nullptr;
}

CommandBuffer& CommandBuffer::operator=(CommandBuffer&& other) noexcept {
    std::swap(m_commandBuffer, other.m_commandBuffer);
    return *this;
}

CommandBuffer::~CommandBuffer() {
    device().freeCommandBuffers(commandPool(), m_commandBuffer);
}

void CommandBuffer::submitToGraphicsQueue(bool waitIdle) const {
    graphicsQueue().submit(vk::SubmitInfo{{}, {}, m_commandBuffer});
    if (waitIdle) {
        graphicsQueue().waitIdle();
    }
}

void CommandBuffer::submitToComputeQueue(bool waitIdle) const {
    computeQueue().submit(vk::SubmitInfo{{}, {}, m_commandBuffer});
    if (waitIdle) {
        computeQueue().waitIdle();
    }
}

}