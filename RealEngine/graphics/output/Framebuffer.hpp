/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/renderer/VulkanObject.hpp>

namespace re {

/**
 * @brief Allows drawing to a texture
 */
class Framebuffer: public VulkanObject {
public:
    explicit Framebuffer();

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
