/*!
 *  @author    Dubsky Tomas
 */
export module RealEngine.rendering.internal_interfaces.IOrdering;

export namespace RE {

/**
* @brief Controls which types of incoherent access need the barrier
*/
enum class IncoherentAccessBarrierFlags : int {
    VERTEX_ATTRIB_ARRAY,
    ELEMENT_ARRAY,
    UNIFORM,
    TEXTURE_FETCH,
    SHADER_IMAGE_ACCESS,
    COMMAND,
    PIXEL_BUFFER,
    TEXTURE_UPDATE,
    BUFFER_UPDATE,
    FRAMEBUFFER,
    TRANSFORM_FEEDBACK,
    ATOMIC_COUNTER, 
    SHADER_STORAGE
};

inline IncoherentAccessBarrierFlags operator|(IncoherentAccessBarrierFlags a, IncoherentAccessBarrierFlags b) {
    return static_cast<IncoherentAccessBarrierFlags>(static_cast<int>(a) | static_cast<int>(b));
}

inline bool operator&(IncoherentAccessBarrierFlags a, IncoherentAccessBarrierFlags b) {
    return static_cast<int>(a) & static_cast<int>(b);
}

/**
* @brief Is a renderer-agnostic interface to Ordering's implementation.
*
* This is used internally by Ordering.
*
* @see RE::Ordering
*/
class IOrdering {
public:

    virtual void flushWork() const = 0;
    virtual void finishWork() const = 0;

    virtual void issueDrawBarrier() const = 0;
    virtual void issueIncoherentAccessBarrier(IncoherentAccessBarrierFlags barriers) const = 0;
};

}