/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IOrdering.hpp>

namespace RE {

/**
* @brief Is OpenGL 4.6 implementation of Ordering
*
* Do not use this directly - use Ordering class instead.
*/
class GL46_Ordering final : public IOrdering {
public:

    void flushWork() const override;
    void finishWork() const override;

    void issueDrawBarrier() const override;
    void issueIncoherentAccessBarrier(IncoherentAccessBarrierFlags barriers) const override;
};

}