#include "Viewport.hpp"

#include <GL/glew.h>

namespace RE {

std::optional<UniformBuffer> Viewport::m_windowMatrixUniformBuffer;
glm::ivec2 Viewport::m_windowSize{};
glm::mat4 Viewport::m_windowMatrix{};

void Viewport::setWindowMatrixToMatchViewport() {
	m_windowMatrixUniformBuffer->overwrite(0u, m_windowMatrix);
}

void Viewport::set(const glm::ivec2& pos, const glm::ivec2& size) {
	glViewport(pos.x, pos.y, size.x, size.y);
}

void Viewport::setToWholeWindow() {
	glViewport(0, 0, m_windowSize.x, m_windowSize.y);
}

}