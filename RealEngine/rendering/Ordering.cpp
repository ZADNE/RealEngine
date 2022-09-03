/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/Ordering.hpp>

#include <RealEngine/rendering/all_renderers.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_Ordering.hpp>

namespace RE {

template<typename R>
void Ordering<R>::flushWork() {
    s_impl->flushWork();
}

template<typename R>
void Ordering<R>::finishWork() {
    s_impl->finishWork();
}

template<typename R>
void Ordering<R>::issueDrawBarrier() {
    s_impl->issueDrawBarrier();
}

template<typename R>
void Ordering<R>::issueIncoherentAccessBarrier(IncoherentAccessBarrierFlags barriers) {
    s_impl->issueIncoherentAccessBarrier(barriers);
}

template Ordering<RendererLateBind>;
template Ordering<RendererGL46>;

}
