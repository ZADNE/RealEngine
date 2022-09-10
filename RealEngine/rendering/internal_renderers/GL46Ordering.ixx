/*!
 *  @author    Dubsky Tomas
 */
export module RealEngine.rendering.internal_renderers.GL46Ordering;
import RealEngine.rendering.internal_interfaces.IOrdering;

export namespace RE {

/**
* @brief Is OpenGL 4.6 implementation of Ordering
*
* Do not use this directly - use Ordering class instead.
*/
class GL46Ordering final : public IOrdering {
public:

    void flushWork() const override;
    void finishWork() const override;

    void issueDrawBarrier() const override;
    void issueIncoherentAccessBarrier(IncoherentAccessBarrierFlags barriers) const override;
};

}