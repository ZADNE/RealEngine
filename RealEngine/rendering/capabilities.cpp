/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/capabilities.hpp>

#include <RealEngine/rendering/RendererLateBind.hpp>
#include <RealEngine/rendering/RendererGL46.hpp>

namespace RE {

template<typename R>
void BlendingCapability<R>::enable() {
    Capabilities<R>::s_impl->set(Capability::BLENDING, true);
}

template<typename R>
void BlendingCapability<R>::disable() {
    Capabilities<R>::s_impl->set(Capability::BLENDING, false);
}

template<typename R>
void BlendingCapability<R>::set(bool enabled) {
    Capabilities<R>::s_impl->set(Capability::BLENDING, enabled);
}

template BlendingCapability<RendererLateBind>;
template BlendingCapability<RendererGL46>;

}
