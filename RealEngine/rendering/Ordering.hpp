/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IOrdering.hpp>
#include <RealEngine/rendering/Renderer.hpp>


namespace RE {

/**
 * @brief Manages ordering of rendering commnads
 * @tparam R The renderer that will perform the commands
*/
template<Renderer R = RendererLateBind>
class Ordering {
    friend class GL46Fixture;
public:

    static void flushWork();
    static void finishWork();

    static void issueDrawBarrier();
    static void issueIncoherentAccessBarrier(IncoherentAccessBarrierFlags barriers);

protected:

    static inline R::Ordering* s_impl = nullptr;
};

}
