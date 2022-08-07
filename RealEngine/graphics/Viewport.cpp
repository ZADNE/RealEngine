/*! 
 *  @author    Dubsky Tomas
 */
#include "Viewport.hpp"

#include <GL/glew.h>

namespace RE {

std::optional<TypedBuffer> Viewport::s_windowMatrixUniformBuffer;
glm::ivec2 Viewport::s_windowSize{};
glm::mat4 Viewport::s_windowMatrix{};

void Viewport::setWindowMatrixToMatchViewport() {
	s_windowMatrixUniformBuffer->overwrite(0u, s_windowMatrix);
}

void Viewport::set(const glm::ivec2& pos, const glm::ivec2& size) {
	glViewport(pos.x, pos.y, size.x, size.y);
}

void Viewport::setToWholeWindow() {
	glViewport(0, 0, s_windowSize.x, s_windowSize.y);
}

}