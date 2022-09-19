/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/ICapabilities.hpp>

namespace RE {

/**
* @brief Is Vulkan 1.3 implementation of capabilities
*
* Do not use this directly - use *Capability classes instead.
*/
class VK13Capabilities final : public ICapabilities {
public:

    void set(Capability cap, bool enabled) const override;
};

}