/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/renderer/VulkanObject.hpp>
#include <concepts>

namespace re {

/**
 * @brief Records commands to be executed by device (= GPU)
 */
class CommandBuffer: public VulkanObject {
public:
    CommandBuffer(vk::CommandBufferLevel level);

    CommandBuffer(const CommandBuffer&)            = delete; /**< Noncopyable */
    CommandBuffer& operator=(const CommandBuffer&) = delete; /**< Noncopyable */

    CommandBuffer(CommandBuffer&& other) noexcept;            /**< Movable */
    CommandBuffer& operator=(CommandBuffer&& other) noexcept; /**< Movable */

    ~CommandBuffer();

    template<typename F>
        requires std::invocable<F, const vk::CommandBuffer&>
    static void doOneTimeSubmit(F op) {
        oneTimeSubmitCommandBuffer().begin(
            {vk::CommandBufferUsageFlagBits::eOneTimeSubmit}
        );
        op(oneTimeSubmitCommandBuffer());
        oneTimeSubmitCommandBuffer().end();
        graphicsQueue().submit(vk::SubmitInfo{
            {}, {}, oneTimeSubmitCommandBuffer()});
        device().waitIdle(); // TODO expensive operation, use fence instead or
                             // use outside main loop
    }

    static void submitToGraphicsQueue(
        const vk::ArrayProxy<const vk::SubmitInfo2>& submits,
        const vk::Fence&                             signalFence = nullptr
    );

    static void submitToComputeQueue(
        const vk::ArrayProxy<const vk::SubmitInfo2>& submits,
        const vk::Fence&                             signalFence = nullptr
    );

    void submitToGraphicsQueue(const vk::Fence& signalFence = nullptr) const;
    void submitToComputeQueue(const vk::Fence& signalFence = nullptr) const;

    const vk::CommandBuffer& operator*() const { return m_commandBuffer; }
    const vk::CommandBuffer* operator->() const { return &m_commandBuffer; }

    const vk::CommandBuffer& commandBuffer() const { return m_commandBuffer; }

private:
    vk::CommandBuffer m_commandBuffer = nullptr;
};

} // namespace re
