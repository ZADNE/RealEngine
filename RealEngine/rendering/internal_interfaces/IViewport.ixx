/*!
 *  @author    Dubsky Tomas
 */
export module RealEngine.rendering.internal_interfaces.IViewport;
import <glm/vec2.hpp>;

export namespace RE {

/**
 * @brief Contains state of Viewport
 * @note For internal usage in RealEngine.
*/
struct ViewportState {
    glm::ivec2 windowSize;
    bool trackingWindow;
};

/**
* @brief Is a renderer-agnostic interface to Viewport's implementation.
*
* This is used internally by Viewport.
*
* @see RE::Viewport
*/
class IViewport {
public:

    virtual void set(const glm::ivec2& pos, const glm::ivec2& size) const = 0;
};

}