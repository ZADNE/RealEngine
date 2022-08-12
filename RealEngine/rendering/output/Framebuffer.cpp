/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/output/Framebuffer.hpp>

namespace RE {

IFramebuffer* Framebuffer::s_impl = nullptr;
Framebuffer Framebuffer::defaultFramebuffer{0};

Framebuffer::Framebuffer() {

}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept :
	m_ID(other.m_ID) {
	other.m_ID = 0;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
	std::swap(m_ID, other.m_ID);
	return *this;
}

}