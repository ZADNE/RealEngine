/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/ICapabilities.hpp>
#include <RealEngine/rendering/Renderer.hpp>

namespace RE {

template<Renderer R = RendererLateBind>
class Capabilities {
    friend class GL46_Fixture;
protected:

    static inline R::Capabilities* s_impl = nullptr;
};

/**
 * @brief Controls mixing of new and old pixels in framebuffer
 * @tparam R The renderer that will perform the commands
 *
 * Enabled means that the incoming color is mixed with the previous based on incoming's alpha.
 * Disabled means that the incoming color simply replaces the previous color.
*/
template<Renderer R = RendererLateBind>
class BlendingCapability : Capabilities<R> {
public:

    static void enable();
    static void disable();
    static void set(bool enabled);
};

}
