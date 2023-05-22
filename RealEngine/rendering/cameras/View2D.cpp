/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/cameras/View2D.hpp>

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include <RealEngine/user_input/InputManager.hpp>

namespace RE {

View2D::View2D(const glm::vec2& viewDimensions):
    m_viewDimensions(viewDimensions) {
    m_orthoMatrix = glm::ortho(0.0f, m_viewDimensions.x, 0.0f, m_viewDimensions.y);
    update();
}

void View2D::setCursorAbs(const glm::vec2& cursorAbs) {
    m_cursorAbs = cursorAbs;
    m_cursorRel = convertAbsToRel(cursorAbs);
}

glm::vec2 View2D::convertAbsToRel(const glm::uvec2& abs) {
    //Center [0;0]
    glm::vec2 floatAbs = glm::vec2(abs);
    floatAbs -= m_viewDimensions / 2.0f;
    //Scaling
    floatAbs /= m_scale;
    //Translation
    floatAbs += m_position;
    return (glm::vec2)floatAbs;
}

void View2D::resizeView(const glm::vec2& newDims) {
    m_viewDimensions = newDims;
    m_orthoMatrix = glm::ortho(0.0f, newDims.x, 0.0f, newDims.y);
    update();
}

void View2D::update() {
    //Clip
    clip();
    //Translate
    glm::vec3 translate(-m_position.x + m_viewDimensions.x / 2.0f, -m_position.y + m_viewDimensions.y / 2.0f, 0.0f);
    m_viewMatrix = glm::translate(m_orthoMatrix, translate);
    //Scale
    glm::vec3 scale(m_scale.x, m_scale.y, 0.0f);
    m_viewMatrix = glm::scale(glm::mat4(1.0f), scale) * m_viewMatrix;
    //Update relative cursor position
    m_cursorRel = convertAbsToRel(m_cursorAbs);
}

void View2D::enableClipping(const glm::vec2& minXY, const glm::vec2& maxXY) {
    m_minXY = minXY;
    m_maxXY = maxXY;
    m_clippingEnabled = true;
}

void View2D::disableClipping() {
    m_clippingEnabled = false;
}

void View2D::clip() {
    if (!m_clippingEnabled) { return; }
    m_position.x = std::clamp(m_position.x, m_minXY.x + m_viewDimensions.x / 2.0f / m_scale.x, m_maxXY.x - m_viewDimensions.x / 2.0f / m_scale.x);
    m_position.y = std::clamp(m_position.y, m_minXY.y + m_viewDimensions.y / 2.0f / m_scale.y, m_maxXY.y - m_viewDimensions.y / 2.0f / m_scale.y);
}

}