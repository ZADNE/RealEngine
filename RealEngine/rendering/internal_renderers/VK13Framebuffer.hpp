/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IFramebuffer.hpp>

namespace RE {

/**
* @brief Is Vulkan 1.3 implementation of Framebuffer
*
* Do not use this directly - use Framebuffer class instead.
*/
class VK13Framebuffer final : public IFramebuffer {
public:

    FramebufferID construct() const override;
    void destruct(FramebufferID& fb) const override;

    void attachImage(FramebufferID& fb, FramebufferAttachment attachment, const TextureID& te, int level) const override;

    void associateAttachementsWithOutputs(FramebufferID& fb, const std::vector<FramebufferOutput>& outputs) const override;
    void selectAttachmentForColorReading(FramebufferID& fb, unsigned int colorAttachmentIndex) const override;

    void targetMe(const FramebufferID& fb, FramebufferTarget target) const override;
    FramebufferTargetability checkTargetability(const FramebufferID& fb, FramebufferTarget target) const override;

    void clearColorAttachment(const FramebufferID& fb, unsigned int attachmentIndex, const glm::vec4& color) const override;
    void clearColorAttachment(const FramebufferID& fb, unsigned int attachmentIndex, const glm::ivec4& color) const override;
    void clearColorAttachment(const FramebufferID& fb, unsigned int attachmentIndex, const glm::uvec4& color) const override;
    void clearDepthAttachment(const FramebufferID& fb, float depth) const override;
    void clearStencilAttachment(const FramebufferID& fb, int stencil) const override;
    void clearDepthAndStencilAttachments(const FramebufferID& fb, float depth, int stencil) const override;
};

}