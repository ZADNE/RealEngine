/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/GL46_Framebuffer.hpp>

#include <GL/glew.h>

namespace RE {

void GL46_Framebuffer::construct(Framebuffer& fb) const {

}

void GL46_Framebuffer::destruct(Framebuffer& fb) const {

}

void GL46_Framebuffer::attachImage(const Framebuffer& fb, const Texture& te, int level, FramebufferAttachment attachment) const {

}

void GL46_Framebuffer::associateAttachementsWithOutputs(const Framebuffer& fb, const std::vector<const FramebufferOutput>& outputs) const {

}

void GL46_Framebuffer::selectAttachmentForColorReading(const Framebuffer& fb, unsigned int colorAttachmentIndex) const {

}

void GL46_Framebuffer::targetMe(const Framebuffer& fb, FramebufferTarget target) const {

}

FramebufferTargetability GL46_Framebuffer::targetability(const Framebuffer& fb, FramebufferTarget target) const {
	return FramebufferTargetability();
}

void GL46_Framebuffer::clearColorAttachment(const Framebuffer& fb, unsigned int attachmentIndex, const glm::vec4& color) const {

}

void GL46_Framebuffer::clearColorAttachment(const Framebuffer& fb, unsigned int attachmentIndex, const glm::ivec4& color) const {

}

void GL46_Framebuffer::clearColorAttachment(const Framebuffer& fb, unsigned int attachmentIndex, const glm::uvec4& color) const {

}

void GL46_Framebuffer::clearDepthAttachment(const Framebuffer& fb, float depth) const {

}

void GL46_Framebuffer::clearStencilAttachment(const Framebuffer& fb, int stencil) const {

}

void GL46_Framebuffer::clearDepthAndStencilAttachments(const Framebuffer& fb, float depth, int stencil) const {

}

}