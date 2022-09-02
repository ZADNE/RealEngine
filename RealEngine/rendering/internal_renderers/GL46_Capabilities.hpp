/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/ICapabilities.hpp>

namespace RE {

/**
* @brief Is OpenGL 4.6 implementation of capabilities
*
* Do not use this directly - use *Capability classes instead.
*/
class GL46_Capabilities final : public ICapabilities {
public:

    void set(Capability cap, bool enabled) const override;
};

}