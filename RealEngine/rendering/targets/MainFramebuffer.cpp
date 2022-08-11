/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/targets/MainFramebuffer.hpp>

namespace RE {

IMainFramebuffer* MainFramebuffer::s_impl = nullptr;

void MainFramebuffer::setClearColor(const glm::vec4& color) {
	s_impl->setClearColor(color);
}

void MainFramebuffer::setClearDepth(float depth) {
	s_impl->setClearDepth(depth);
}

void MainFramebuffer::setClearDepth(double depth) {
	s_impl->setClearDepth(depth);
}

void MainFramebuffer::setClearStencil(int stencil) {
	s_impl->setClearStencil(stencil);
}

void MainFramebuffer::clear(ClearFlags mask) {
	s_impl->clear(mask);
}

}