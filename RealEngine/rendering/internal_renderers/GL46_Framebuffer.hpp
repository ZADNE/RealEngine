/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IFramebuffer.hpp>

namespace RE {

/**
* @brief Is OpenGL 4.6 implementation of Framebuffer
*
* Do not use this directly - use Framebuffer class instead.
*/
class GL46_Framebuffer final : public IFramebuffer {
public:

    FramebufferInternals construct() const override;
    void destruct(FramebufferInternals& fb) const override;

    void attachImage(FramebufferInternals& fb, FramebufferAttachment attachment, const TextureInternals& te, int level) const override;

    void associateAttachementsWithOutputs(FramebufferInternals& fb, const std::vector<FramebufferOutput>& outputs) const override;
    void selectAttachmentForColorReading(FramebufferInternals& fb, unsigned int colorAttachmentIndex) const override;

    void targetMe(const FramebufferInternals& fb, FramebufferTarget target) const override;
    FramebufferTargetability checkTargetability(const FramebufferInternals& fb, FramebufferTarget target) const override;

    void clearColorAttachment(const FramebufferInternals& fb, unsigned int attachmentIndex, const glm::vec4& color) const override;
    void clearColorAttachment(const FramebufferInternals& fb, unsigned int attachmentIndex, const glm::ivec4& color) const override;
    void clearColorAttachment(const FramebufferInternals& fb, unsigned int attachmentIndex, const glm::uvec4& color) const override;
    void clearDepthAttachment(const FramebufferInternals& fb, float depth) const override;
    void clearStencilAttachment(const FramebufferInternals& fb, int stencil) const override;
    void clearDepthAndStencilAttachments(const FramebufferInternals& fb, float depth, int stencil) const override;
};

}