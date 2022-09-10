/*!
 *  @author    Dubsky Tomas
 */
export module RealEngine.rendering.output.Viewport;
import glm;
export import RealEngine.rendering.internal_interfaces.IViewport;
export import RealEngine.rendering.Renderer;


export namespace RE {

/**
 * @brief Controls transformation from Normalized device coordinates to Window coordinates
 * @tparam R The renderer that will perform the commands
*/
template<Renderer R = RendererLateBind>
class Viewport {
    friend class Window;
    friend class GL46Fixture;
public:

    /**
     * @brief Sets viewport rectangle for further rendering
     * @param pos Position of the rectangle
     * @param size Size of the rectangle
    */
    static void set(const glm::ivec2& pos, const glm::ivec2& size);

    /**
     * @brief Sets viewport rectangle to match shole Window
    */
    static void setToWholeWindow();

private:

    static inline ViewportState* s_state = nullptr;

    static inline R::Viewport* s_impl = nullptr;
};

}