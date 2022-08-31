/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IOrdering.hpp>

namespace RE {

class RendererLateBind;

/**
 * @brief Manages ordering of rendering commnads
 * @tparam R The renderer that will perform the commands
*/
template<typename R = RendererLateBind>
class Ordering {
    friend class GL46_Fixture;
public:

    static void flushWork();
    static void finishWork();

    static void issueDrawBarrier();
    static void issueIncoherentAccessBarrier(IncoherentAccessBarrierFlags barriers);

protected:

    static inline R::Ordering* s_impl = nullptr;
};

}
