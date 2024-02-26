/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

namespace re {

/**
 * @brief Represents an orthographic projection.
 */
class View2D {
public:
    explicit View2D(glm::vec2 viewDimensions);

    void setCursorAbs(glm::vec2 cursorAbs);

    glm::vec2 convertAbsToRel(glm::uvec2 abs);

    void resizeView(glm::vec2 newDims);
    void setPosition(glm::vec2 newPosition) {
        m_position = newPosition;
        update();
    }
    void setScale(glm::vec2 newScale) {
        m_scale = newScale;
        update();
    }
    void shiftPosition(glm::vec2 shiftPosition) {
        m_position += shiftPosition;
        update();
    }
    void zoom(glm::vec2 zoomVec) {
        m_scale *= zoomVec;
        update();
    }

    glm::vec2 center() const { return m_position; };
    glm::vec2 botLeft() const { return m_position - scaledDims() / 2.0f; };
    glm::vec2 scale() const { return m_scale; };
    glm::vec2 unscaledDims() const { return m_viewDimensions; }
    glm::vec2 scaledDims() const { return m_viewDimensions / m_scale; };

    /**
     * @brief Position of cursor relative to the view
     */
    glm::vec2 cursorRel() { return m_cursorRel; }

    /**
     * @brief Gets the projection matrix of the view
     */
    glm::mat4 viewMatrix() const { return m_viewMatrix; };

    void enableClipping(glm::vec2 minXY, glm::vec2 maxXY);
    void disableClipping();

private:
    void update();

    glm::vec2 m_position = glm::vec2(0.0f, 0.0f);
    glm::vec2 m_scale    = glm::vec2(1.0f, 1.0f);

    glm::mat4 m_viewMatrix  = glm::mat4(1.0f);
    glm::mat4 m_orthoMatrix = glm::mat4(1.0f);

    glm::vec2 m_viewDimensions = glm::vec2(0.0f, 0.0f);

    glm::vec2 m_cursorAbs = glm::vec2(0.0f, 0.0f);
    glm::vec2 m_cursorRel = glm::vec2(0.0f, 0.0f);

    // Clipping
    void clip();

    glm::vec2 m_minXY;
    glm::vec2 m_maxXY;
    bool m_clippingEnabled = false;
};

} // namespace re
