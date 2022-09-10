/*!
 *  @author    Dubsky Tomas
 */
module RealEngine.rendering.Capabilities;
import RealEngine.rendering.internal_renderers.GL46Capabilities;

namespace RE {

template<Renderer R>
void BlendingCapability<R>::enable() {
    Capabilities<R>::s_impl->set(Capability::BLENDING, true);
}

template<Renderer R>
void BlendingCapability<R>::disable() {
    Capabilities<R>::s_impl->set(Capability::BLENDING, false);
}

template<Renderer R>
void BlendingCapability<R>::set(bool enabled) {
    Capabilities<R>::s_impl->set(Capability::BLENDING, enabled);
}

template BlendingCapability<RendererLateBind>;
template BlendingCapability<RendererGL46>;

}
