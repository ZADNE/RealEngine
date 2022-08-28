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
class GL46_Framebuffer : public IFramebuffer {
public:

    void construct(Framebuffer& fb) const override;
    void destruct(Framebuffer& fb) const override;

    void attachImage(Framebuffer& fb, FramebufferAttachment attachment, const Texture& te, int level) const override;

    void associateAttachementsWithOutputs(Framebuffer& fb, const std::vector<FramebufferOutput>& outputs) const override;
    void selectAttachmentForColorReading(Framebuffer& fb, unsigned int colorAttachmentIndex) const override;

    void targetMe(const Framebuffer& fb, FramebufferTarget target) const override;
    FramebufferTargetability checkTargetability(const Framebuffer& fb, FramebufferTarget target) const override;

    void clearColorAttachment(const Framebuffer& fb, unsigned int attachmentIndex, const glm::vec4& color) const override;
    void clearColorAttachment(const Framebuffer& fb, unsigned int attachmentIndex, const glm::ivec4& color) const override;
    void clearColorAttachment(const Framebuffer& fb, unsigned int attachmentIndex, const glm::uvec4& color) const override;
    void clearDepthAttachment(const Framebuffer& fb, float depth) const override;
    void clearStencilAttachment(const Framebuffer& fb, int stencil) const override;
    void clearDepthAndStencilAttachments(const Framebuffer& fb, float depth, int stencil) const override;

};

}