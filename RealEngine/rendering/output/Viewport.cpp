/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/output/Viewport.hpp>

namespace RE {

std::optional<TypedBuffer> Viewport::s_windowMatrixUniformBuffer;
glm::ivec2 Viewport::s_windowSize{};
glm::mat4 Viewport::s_windowMatrix{};
IViewport* Viewport::s_impl = nullptr;

void Viewport::setWindowMatrixToMatchViewport() {
    s_windowMatrixUniformBuffer->overwrite(0u, s_windowMatrix);
}

void Viewport::set(const glm::ivec2& pos, const glm::ivec2& size) {
    s_impl->set(pos, size);
}

void Viewport::setToWholeWindow() {
    s_impl->set(glm::ivec2{0, 0}, s_windowSize);
}

}