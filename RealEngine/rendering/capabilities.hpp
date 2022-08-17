/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/ICapabilities.hpp>

namespace RE {

class Capabilities {
	friend class GL46_Renderer;
protected:
	static ICapabilities* s_impl;
};

/**
 * @brief Controls mixing of new and old pixels in framebuffer
 * 
 * Enabled means that the incoming color is mixed with the previous based on incoming's alpha.
 * Disabled means that the incoming color simply replaces the previous color.
*/
class BlendingCapability : Capabilities {
public:
	static void enable();
	static void disable();
	static void set(bool enabled);
};

}
