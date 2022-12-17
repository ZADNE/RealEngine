/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/CommandBuffer.hpp>


namespace RE {

CommandBuffer::CommandBuffer(vk::CommandBufferLevel level) :
    m_commandBuffer(s_device->allocateCommandBuffers(vk::CommandBufferAllocateInfo{*s_commandPool, level, 1u}).back()) {
}

CommandBuffer::CommandBuffer(CommandBuffer&& other) noexcept :
    m_commandBuffer(other.m_commandBuffer) {
    other.m_commandBuffer = nullptr;
}

CommandBuffer& CommandBuffer::operator=(CommandBuffer&& other) noexcept {
    std::swap(m_commandBuffer, other.m_commandBuffer);
    return *this;
}

CommandBuffer::~CommandBuffer() {
    s_device->freeCommandBuffers(*s_commandPool, m_commandBuffer);
}

void CommandBuffer::submitToGraphicsQueue() const {
    s_graphicsQueue->submit(vk::SubmitInfo{{}, {}, m_commandBuffer});
    s_device->waitIdle();//TODO
}

}