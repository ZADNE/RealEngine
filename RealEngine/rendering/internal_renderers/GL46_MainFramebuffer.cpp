/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/GL46_MainFramebuffer.hpp>

#include <GL/glew.h>

namespace RE {

void GL46_MainFramebuffer::setClearColor(const glm::vec4& color) const {
	glClearColor(color.r, color.g, color.b, color.a);
}

void GL46_MainFramebuffer::setClearDepth(float depth) const {
	glClearDepthf(depth);
}

void GL46_MainFramebuffer::setClearDepth(double depth) const {
	glClearDepth(depth);
}

void GL46_MainFramebuffer::setClearStencil(int stencil) const {
	glClearStencil(stencil);
}

void GL46_MainFramebuffer::clear(ClearFlags mask) const {
	GLbitfield glMask = 0;
	if (mask & ClearFlags::COLOR) {
		glMask |= GL_COLOR_BUFFER_BIT;
	}
	if (mask & ClearFlags::DEPTH) {
		glMask |= GL_DEPTH_BUFFER_BIT;
	}
	if (mask & ClearFlags::STENCIL) {
		glMask |= GL_STENCIL_BUFFER_BIT;
	}
	glClear(glMask);
}

}