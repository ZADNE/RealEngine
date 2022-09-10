/*!
 *  @author    Dubsky Tomas
 */
module RealEngine.rendering.Ordering;
import RealEngine.rendering.internal_renderers.GL46Ordering;

namespace RE {

template<Renderer R>
void Ordering<R>::flushWork() {
    s_impl->flushWork();
}

template<Renderer R>
void Ordering<R>::finishWork() {
    s_impl->finishWork();
}

template<Renderer R>
void Ordering<R>::issueDrawBarrier() {
    s_impl->issueDrawBarrier();
}

template<Renderer R>
void Ordering<R>::issueIncoherentAccessBarrier(IncoherentAccessBarrierFlags barriers) {
    s_impl->issueIncoherentAccessBarrier(barriers);
}

template Ordering<RendererLateBind>;
template Ordering<RendererGL46>;

}
