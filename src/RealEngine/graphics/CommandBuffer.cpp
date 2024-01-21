﻿/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/CommandBuffer.hpp>

namespace re {

CommandBuffer::CommandBuffer(vk::CommandBufferLevel level)
    : m_cmdBuf(device()
                   .allocateCommandBuffers(vk::CommandBufferAllocateInfo{
                       commandPool(), level, 1u})
                   .back()) {
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

void CommandBuffer::beginDebugUtilsLabel(const char* label, glm::vec4 color /* = {}*/) const {
    if constexpr (k_buildType == BuildType::Debug) {
        m_cmdBuf.beginDebugUtilsLabelEXT(
            vk::DebugUtilsLabelEXT{label, {color.r, color.g, color.b, color.a}},
            dispatchLoaderDynamic()
        );
    }
}

void CommandBuffer::endDebugUtilsLabel() const {
    if constexpr (k_buildType == BuildType::Debug) {
        m_cmdBuf.endDebugUtilsLabelEXT(dispatchLoaderDynamic());
    }
}

void CommandBuffer::insertDebugUtilsLabel(const char* label, glm::vec4 color /* = {}*/) const {
    if constexpr (k_buildType == BuildType::Debug) {
        m_cmdBuf.insertDebugUtilsLabelEXT(
            vk::DebugUtilsLabelEXT{label, {color.r, color.g, color.b, color.a}},
            dispatchLoaderDynamic()
        );
    }
}

void CommandBuffer::submitToGraphicsQueue(const vk::Fence& signalFence /* = nullptr*/) const {
    graphicsQueue().submit(vk::SubmitInfo{{}, {}, m_cmdBuf}, signalFence);
}

void CommandBuffer::submitToComputeQueue(const vk::Fence& signalFence /* = nullptr*/) const {
    computeQueue().submit(vk::SubmitInfo{{}, {}, m_cmdBuf}, signalFence);
}

} // namespace re