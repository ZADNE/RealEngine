/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/VK13Framebuffer.hpp>

namespace RE {

FramebufferID VK13Framebuffer::construct() const {
    return FramebufferID{0};
}

void VK13Framebuffer::destruct(FramebufferID& fb) const {

}

void VK13Framebuffer::attachImage(FramebufferID& fb, FramebufferAttachment attachment, const TextureID& te, int level) const {

}

void VK13Framebuffer::associateAttachementsWithOutputs(FramebufferID& fb, const std::vector<FramebufferOutput>& outputs) const {

}

void VK13Framebuffer::selectAttachmentForColorReading(FramebufferID& fb, unsigned int colorAttachmentIndex) const {

}

void VK13Framebuffer::targetMe(const FramebufferID& fb, FramebufferTarget target) const {

}

FramebufferTargetability VK13Framebuffer::checkTargetability(const FramebufferID& fb, FramebufferTarget target) const {
    return FramebufferTargetability::NOT_COMPLETE;
}

void VK13Framebuffer::clearColorAttachment(const FramebufferID& fb, unsigned int attachmentIndex, const glm::vec4& color) const {

}

void VK13Framebuffer::clearColorAttachment(const FramebufferID& fb, unsigned int attachmentIndex, const glm::ivec4& color) const {

}

void VK13Framebuffer::clearColorAttachment(const FramebufferID& fb, unsigned int attachmentIndex, const glm::uvec4& color) const {

}

void VK13Framebuffer::clearDepthAttachment(const FramebufferID& fb, float depth) const {

}

void VK13Framebuffer::clearStencilAttachment(const FramebufferID& fb, int stencil) const {

}

void VK13Framebuffer::clearDepthAndStencilAttachments(const FramebufferID& fb, float depth, int stencil) const {

}

}