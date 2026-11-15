/**
 *  @author    Dubsky Tomas
 */
module realengine:graphics.output_control.Framebuffer_impl;
import :graphics.output_control.Framebuffer;

namespace re {

Framebuffer::Framebuffer(const vk::FramebufferCreateInfo& createInfo)
    : m_framebuffer(device().createFramebuffer(createInfo)) {
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    : m_framebuffer(std::exchange(other.m_framebuffer, nullptr)) {
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
    std::swap(m_framebuffer, other.m_framebuffer);
    return *this;
}

Framebuffer::~Framebuffer() {
    deletionQueue().enqueueDeletion(m_framebuffer);
}

} // namespace re
