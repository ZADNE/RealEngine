/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/capabilities.hpp>

namespace RE {

ICapabilities* Capabilities::s_impl = nullptr;

void BlendingCapability::enable() {
	s_impl->set(Capability::BLENDING, true);
}

void BlendingCapability::disable() {
	s_impl->set(Capability::BLENDING, false);
}

void BlendingCapability::set(bool enabled) {
	s_impl->set(Capability::BLENDING, enabled);
}

}
