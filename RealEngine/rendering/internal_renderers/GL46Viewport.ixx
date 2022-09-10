/*!
 *  @author    Dubsky Tomas
 */
export module RealEngine.rendering.internal_renderers.GL46Viewport;
import RealEngine.rendering.internal_interfaces.IViewport;

export namespace RE {

/**
* @brief Is OpenGL 4.6 implementation of Viewport class.
*
* Do not use this directly - use Viewport instead.
*/
class GL46Viewport final : public IViewport {
public:

    void set(const glm::ivec2& pos, const glm::ivec2& size) const override;
};

}