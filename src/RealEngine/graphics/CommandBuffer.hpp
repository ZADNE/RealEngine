﻿/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <concepts>

#include <RealEngine/renderer/VulkanObject.hpp>

namespace re {

/**
 * @brief Records commands to be executed by device (= GPU)
 */
class CommandBuffer: public VulkanObject {
public:
    /**
     * @brief Constructs a null CommandBuffer that cannot be used for recording
     */
    explicit CommandBuffer() {}

    explicit CommandBuffer(vk::CommandBufferLevel level);

    CommandBuffer(const CommandBuffer&)            = delete; /**< Noncopyable */
    CommandBuffer& operator=(const CommandBuffer&) = delete; /**< Noncopyable */

    CommandBuffer(CommandBuffer&& other) noexcept;            /**< Movable */
    CommandBuffer& operator=(CommandBuffer&& other) noexcept; /**< Movable */

    ~CommandBuffer();

    /**
     * @warning Waits for device to become idle which is very expensive!
     *          Use only when performance is not critical (e.g. outside of main loop)
     */
    template<typename F>
        requires std::invocable<F, const CommandBuffer&>
    static void doOneTimeSubmit(F op) {
        oneTimeSubmitCmdBuf()->begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit}
        );
        op(oneTimeSubmitCmdBuf());
        oneTimeSubmitCmdBuf()->end();
        graphicsQueue().submit(vk::SubmitInfo{{}, {}, *oneTimeSubmitCmdBuf()});
        device().waitIdle();
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

    const vk::CommandBuffer& operator*() const { return m_cmdBuf; }
    const vk::CommandBuffer* operator->() const { return &m_cmdBuf; }

    const vk::CommandBuffer& commandBuffer() const { return m_cmdBuf; }

private:
    vk::CommandBuffer m_cmdBuf{};
};

} // namespace re
