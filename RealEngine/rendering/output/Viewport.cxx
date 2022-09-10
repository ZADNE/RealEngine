/*!
 *  @author    Dubsky Tomas
 */
module RealEngine.rendering.output.Viewport;
import RealEngine.rendering.internal_renderers.GL46Viewport;


namespace RE {

template<Renderer R>
void Viewport<R>::set(const glm::ivec2& pos, const glm::ivec2& size) {
    s_impl->set(pos, size);
    s_state->trackingWindow = false;
}

template<Renderer R>
void Viewport<R>::setToWholeWindow() {
    s_impl->set(glm::ivec2{0, 0}, s_state->windowSize);
    s_state->trackingWindow = true;
}

template Viewport<RendererLateBind>;
template Viewport<RendererGL46>;

}