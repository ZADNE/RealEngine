/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IFramebuffer.hpp>

namespace RE {

/**
 * @brief Controls transformation from Normalized device coordinates to Window coordinates
*/
class GL46_Framebuffer : public IFramebuffer {
public:

	void construct(Framebuffer& fb) const override;
	void destruct(Framebuffer& fb) const override;

	void attachImage(const Framebuffer& fb, const Texture& te, int level, FramebufferAttachment attachment) const override;

	void associateAttachementsWithOutputs(const Framebuffer& fb, const std::vector<const FramebufferOutput>& outputs) const override;
	void selectAttachmentForColorReading(const Framebuffer& fb, unsigned int colorAttachmentIndex) const override;

	void targetMe(const Framebuffer& fb, FramebufferTarget target) const override;
	FramebufferTargetability targetability(const Framebuffer& fb, FramebufferTarget target) const override;

	void clearColorAttachment(const Framebuffer& fb, unsigned int attachmentIndex, const glm::vec4& color) const override;
	void clearColorAttachment(const Framebuffer& fb, unsigned int attachmentIndex, const glm::ivec4& color) const override;
	void clearColorAttachment(const Framebuffer& fb, unsigned int attachmentIndex, const glm::uvec4& color) const override;
	void clearDepthAttachment(const Framebuffer& fb, float depth) const override;
	void clearStencilAttachment(const Framebuffer& fb, int stencil) const override;
	void clearDepthAndStencilAttachments(const Framebuffer& fb, float depth, int stencil) const override;

};

}