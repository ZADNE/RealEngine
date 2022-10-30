/*!
 *  @author    Dubsky Tomas
 */
#pragma once

namespace RE {

/**
* @brief Controls which types of incoherent access need the barrier
*/
enum class IncoherentAccessBarrierFlags : int {
    VERTEX_ATTRIB_ARRAY =   1 << 0,
    ELEMENT_ARRAY =         1 << 1,
    UNIFORM =               1 << 2,
    TEXTURE_FETCH =         1 << 3,
    SHADER_IMAGE_ACCESS =   1 << 4,
    COMMAND =               1 << 5,
    PIXEL_BUFFER =          1 << 6,
    TEXTURE_UPDATE =        1 << 7,
    BUFFER_UPDATE =         1 << 8,
    FRAMEBUFFER =           1 << 9,
    TRANSFORM_FEEDBACK =    1 << 10,
    ATOMIC_COUNTER =        1 << 11,
    SHADER_STORAGE =        1 << 12
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