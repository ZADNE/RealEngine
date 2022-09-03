/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/output/Viewport.hpp>

#include <RealEngine/rendering/RendererLateBind.hpp>
#include <RealEngine/rendering/RendererGL46.hpp>

namespace RE {

template <typename R>
void Viewport<R>::set(const glm::ivec2& pos, const glm::ivec2& size) {
    s_impl->set(pos, size);
    s_state->trackingWindow = false;
}

template <typename R>
void Viewport<R>::setToWholeWindow() {
    s_impl->set(glm::ivec2{0, 0}, s_state->windowSize);
    s_state->trackingWindow = true;
}

template Viewport<RendererLateBind>;
template Viewport<RendererGL46>;

}