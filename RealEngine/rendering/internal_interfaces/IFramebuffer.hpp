/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <type_traits>
#include <vector>

#include <glm/vec4.hpp>

namespace RE {

/**
 * @brief Constrols what operations is the buffer target for
*/
enum class FramebufferTarget {
    DRAWING,            /**< Target for drawing only */
    READING,            /**< Target for reading only */
    DRAWING_AND_READING /**< Target for both */
};

/**
 * @brief Controls what attachments is a texture attached to
*/
enum class FramebufferAttachment : unsigned int {
    COLOR0 = 0,                         /**< Attached to color attachment n. 0 */
    COLOR1,
    COLOR2,
    COLOR3,
    COLOR4,
    COLOR5,
    COLOR6,
    COLOR7,
    DEPTH = 0x100,                      /**< Attached to depth attachment */
    STENCIL = 0x200,                    /**< Attached to stencil attachment */
    DEPTH_AND_STENCIL = DEPTH | STENCIL /**< Attached to depth and stencil attachment */
};

/**
 * @brief Calculates enum value for n-th color attachment
*/
inline FramebufferAttachment FramebufferAttachmentColorN(unsigned int n) {
    return static_cast<FramebufferAttachment>(static_cast<unsigned int>(FramebufferAttachment::COLOR0) + n);
}

/**
 * @brief Controls which attachment receives the indexed output
 * @warning It is not possible for multiple outputs to write to the same attachment!
*/
enum class FramebufferOutput : unsigned int {
    TO_COLOR0 = 0x400,                  /**< Color n. 0 attachment receives the output */
    TO_COLOR1,
    TO_COLOR2,
    TO_COLOR3,
    TO_COLOR4,
    TO_COLOR5,
    TO_COLOR6,
    TO_COLOR7,
    DISCARD = 0x800                     /**< The output is discarded */
};

/**
 * @brief Calculates enum value for n-th color output
*/
inline FramebufferOutput FramebufferOutputN(unsigned int n) {
    return static_cast<FramebufferOutput>(static_cast<unsigned int>(FramebufferOutput::TO_COLOR0) + n);
}

/**
 * @brief Used to check whether framebuffer can be used as target
*/
enum class FramebufferTargetability {
    TARGETABLE,                         /**< The framebuffer can be used as target */
    NOT_COMPLETE
};

class Texture;

/**
 * @brief Contains all members of Framebuffer
 * @note For internal usage in RealEngine.
*/
class FramebufferInternals {
    friend class GL46_Framebuffer;
    friend class GL46_Fixture;
public:

    ~FramebufferInternals() = default;

    FramebufferInternals(const FramebufferInternals&) = delete;
    FramebufferInternals(FramebufferInternals&& other) noexcept :
        m_id(other.m_id) {
        other.m_id = 0;
    }

    FramebufferInternals& operator=(const FramebufferInternals&) = delete;
    FramebufferInternals& operator=(FramebufferInternals&& other) noexcept {
        std::swap(m_id, other.m_id);
        return *this;
    }

private:

    FramebufferInternals(unsigned int id) :
        m_id(id) {}

    unsigned int m_id = 0;      /**< Internal identifier */
};

/**
* @brief Is a renderer-agnostic interface to Framebuffer's implementation.
*
* This is used internally by Framebuffer.
*
* @see RE::Framebuffer
*/
class IFramebuffer {
public:

    virtual FramebufferInternals construct() const = 0;
    virtual void destruct(FramebufferInternals& fb) const = 0;

    virtual void attachImage(FramebufferInternals& fb, FramebufferAttachment attachment, const Texture& te, int level) const = 0;

    virtual void associateAttachementsWithOutputs(FramebufferInternals& fb, const std::vector<FramebufferOutput>& outputs) const = 0;
    virtual void selectAttachmentForColorReading(FramebufferInternals& fb, unsigned int colorAttachmentIndex) const = 0;

    virtual void targetMe(const FramebufferInternals& fb, FramebufferTarget target) const = 0;
    virtual FramebufferTargetability checkTargetability(const FramebufferInternals& fb, FramebufferTarget target) const = 0;

    virtual void clearColorAttachment(const FramebufferInternals& fb, unsigned int attachmentIndex, const glm::vec4& color) const = 0;
    virtual void clearColorAttachment(const FramebufferInternals& fb, unsigned int attachmentIndex, const glm::ivec4& color) const = 0;
    virtual void clearColorAttachment(const FramebufferInternals& fb, unsigned int attachmentIndex, const glm::uvec4& color) const = 0;
    virtual void clearDepthAttachment(const FramebufferInternals& fb, float depth) const = 0;
    virtual void clearStencilAttachment(const FramebufferInternals& fb, int stencil) const = 0;
    virtual void clearDepthAndStencilAttachments(const FramebufferInternals& fb, float depth, int stencil) const = 0;

};

}