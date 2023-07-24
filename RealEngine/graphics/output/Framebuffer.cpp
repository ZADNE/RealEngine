/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/output/Framebuffer.hpp>

namespace re {

Framebuffer::Framebuffer() {
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    : m_framebuffer(other.m_framebuffer) {
    other.m_framebuffer = nullptr;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
    std::swap(m_framebuffer, other.m_framebuffer);
    return *this;
}

Framebuffer::~Framebuffer() {
    deletionQueue().enqueueDeletion(m_framebuffer);
}

} // namespace re
