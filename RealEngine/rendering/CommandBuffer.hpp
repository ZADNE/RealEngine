/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <concepts>

#include <vulkan/vulkan.hpp>

namespace RE {

class CommandBuffer {
    friend class VK13Fixture;
public:

    CommandBuffer(vk::CommandBufferLevel level);

    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer& operator=(const CommandBuffer&) = delete;

    CommandBuffer(CommandBuffer&& other) noexcept;
    CommandBuffer& operator=(CommandBuffer&& other) noexcept;

    ~CommandBuffer();

    template <typename F> requires std::invocable<F, const vk::CommandBuffer&>
    static void doOneTimeSubmit(F op) {
        s_oneTimeSubmitCommandBuffer->begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
        op(*s_oneTimeSubmitCommandBuffer);
        s_oneTimeSubmitCommandBuffer->end();
        s_graphicsQueue->submit(vk::SubmitInfo{{}, {}, *s_oneTimeSubmitCommandBuffer});
        s_device->waitIdle();//TODO expensive operation, use fence/semaphore instead
    }

    void submitToGraphicsQueue() const;

    const vk::CommandBuffer& operator*() { return m_commandBuffer; }
    const vk::CommandBuffer* operator->() { return &m_commandBuffer; }

private:

    vk::CommandBuffer m_commandBuffer = nullptr;

    static inline const vk::Device* s_device = nullptr;
    static inline const vk::CommandPool* s_commandPool = nullptr;
    static inline const vk::Queue* s_graphicsQueue = nullptr;
    static inline const vk::CommandBuffer* s_oneTimeSubmitCommandBuffer = nullptr;
};

}
