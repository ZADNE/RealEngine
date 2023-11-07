/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/renderer/VulkanObject.hpp>

namespace re {

/**
 * @brief Represents a collection of specific memory attachments,
 * to be used by a RenderPass.
 */
class Framebuffer: public VulkanObject {
public:
    /**
     * @brief Constructs a null framebuffer that cannot be rendered into
     */
    explicit Framebuffer() {}

    /**
     * @brief Constructs a framebuffer that can be used as rendering target
     */
    explicit Framebuffer(const vk::FramebufferCreateInfo& createInfo);

    Framebuffer(const Framebuffer&)            = delete; /**< Noncopyable */
    Framebuffer& operator=(const Framebuffer&) = delete; /**< Noncopyable */

    Framebuffer(Framebuffer&& other) noexcept;            /**< Movable */
    Framebuffer& operator=(Framebuffer&& other) noexcept; /**< Movable */

    ~Framebuffer();

    const vk::Framebuffer& operator*() const { return m_framebuffer; }
    const vk::Framebuffer* operator->() const { return &m_framebuffer; }

    const vk::Framebuffer& framebuffer() const { return m_framebuffer; }

private:
    vk::Framebuffer m_framebuffer{};
};

} // namespace re
