/*! 
 *  @author    Dubsky Tomas
 */
export module RealEngine.rendering.cameras.View2D;
import glm;


export namespace RE {

/**
 * @brief Represents an orthographic projection.
*/
class View2D {
public:

    View2D(const glm::vec2& viewDimensions);

    void resizeView(const glm::vec2& newDims);

    void setCursorAbs(const glm::vec2& cursorAbs);

    //Cursor
    glm::vec2 convertAbsToRel(const glm::uvec2& abs);
    glm::vec2 getCursorRel();


    //Setters
    void setPosition(const glm::vec2& newPosition) { m_position = newPosition; update(); }
    void setScale(const glm::vec2& newScale) { m_scale = newScale; update(); }

    //Changers
    void shiftPosition(const glm::vec2& shiftPosition) { m_position += shiftPosition; update(); }
    //Scale is multiplied by zoomVec
    void zoom(const glm::vec2& zoomVec) { m_scale *= zoomVec; update(); }

    //Getters

    //Center of the view
    glm::vec2 getPosition() const { return m_position; };
    //Bottom-left corner of the view
    glm::vec2 getBotLeft() const { return m_position - getScaledDims() / 2.0f; };
    glm::vec2 getScale() const { return m_scale; };
    glm::vec2 getUnscaledDims() const { return m_viewDimensions; }
    glm::vec2 getScaledDims() const { return m_viewDimensions / m_scale; };
    glm::mat4 getViewMatrix() const { return m_viewMatrix; };

    //View clipping
    void enableClipping(const glm::vec2& minXY, const glm::vec2& maxXY);
    void disableClipping();

private:

    void update();

    glm::vec2 m_position = glm::vec2(0.0f, 0.0f);
    glm::vec2 m_scale = glm::vec2(1.0f, 1.0f);

    glm::mat4 m_viewMatrix = glm::mat4(1.0f);
    glm::mat4 m_orthoMatrix = glm::mat4(1.0f);

    glm::vec2 m_viewDimensions = glm::vec2(0.0f, 0.0f);

    glm::vec2 m_cursorAbs = glm::vec2(0.0f, 0.0f);
    glm::vec2 m_cursorRel = glm::vec2(0.0f, 0.0f);

    //Clipping
    void clip();

    glm::vec2 m_minXY;
    glm::vec2 m_maxXY;
    bool m_clippingEnabled = false;
};

}