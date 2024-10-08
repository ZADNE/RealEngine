﻿/**
 *  @author    Dubsky Tomas
 */
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include <RealEngine/graphics/cameras/View2D.hpp>
#include <RealEngine/user_input/InputManager.hpp>

namespace re {

View2D::View2D(glm::vec2 viewDimensions)
    : m_viewDimensions(viewDimensions) {
    m_orthoMatrix = glm::ortho(0.0f, m_viewDimensions.x, 0.0f, m_viewDimensions.y);
    update();
}

void View2D::setCursorAbs(glm::vec2 cursorAbs) {
    m_cursorAbs = cursorAbs;
    m_cursorRel = convertAbsToRel(cursorAbs);
}

glm::vec2 View2D::convertAbsToRel(glm::uvec2 abs) const {
    // Center [0;0]
    glm::vec2 floatAbs = glm::vec2(abs);
    floatAbs -= m_viewDimensions * 0.5f;
    // Scaling
    floatAbs /= m_scale;
    // Translation
    floatAbs += m_position;
    return (glm::vec2)floatAbs;
}

void View2D::resizeView(glm::vec2 newDims) {
    m_viewDimensions = newDims;
    m_orthoMatrix    = glm::ortho(0.0f, newDims.x, 0.0f, newDims.y);
    update();
}

void View2D::update() {
    // Clip
    clip();
    // Translate
    glm::vec3 translate(
        -m_position.x + m_viewDimensions.x * 0.5f,
        -m_position.y + m_viewDimensions.y * 0.5f, 0.0f
    );
    m_viewMatrix = glm::translate(m_orthoMatrix, translate);
    // Scale
    glm::vec3 scale(m_scale.x, m_scale.y, 0.0f);
    m_viewMatrix = glm::scale(glm::mat4(1.0f), scale) * m_viewMatrix;
    // Update relative cursor position
    m_cursorRel = convertAbsToRel(m_cursorAbs);
}

void View2D::enableClipping(glm::vec2 minXY, glm::vec2 maxXY) {
    m_minXY           = minXY;
    m_maxXY           = maxXY;
    m_clippingEnabled = true;
}

void View2D::disableClipping() {
    m_clippingEnabled = false;
}

void View2D::clip() {
    if (!m_clippingEnabled) {
        return;
    }
    m_position = glm::clamp(
        m_position, m_minXY + m_viewDimensions / m_scale * 0.5f,
        m_maxXY - m_viewDimensions / m_scale * 0.5f
    );
}

} // namespace re
