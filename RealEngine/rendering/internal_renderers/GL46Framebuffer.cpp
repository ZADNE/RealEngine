/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/GL46Framebuffer.hpp>

#include <RealEngine/rendering/output/Framebuffer.hpp>
#include <RealEngine/rendering/textures/Texture.hpp>
#include <RealEngine/utility/Error.hpp>

#include <GL/glew.h>

namespace RE {

GLenum convert(FramebufferTarget target) {
    switch (target) {
    case FramebufferTarget::DRAWING: return GL_DRAW_FRAMEBUFFER;
    case FramebufferTarget::READING: return GL_READ_FRAMEBUFFER;
    case FramebufferTarget::DRAWING_AND_READING: return GL_FRAMEBUFFER;
    default: fatalError("Bad enum value of FramebufferTarget!");
    }
}

FramebufferID GL46Framebuffer::construct() const {
    unsigned int id;
    glCreateFramebuffers(1, &id);
    return FramebufferID{id};
}

void GL46Framebuffer::destruct(FramebufferID& fb) const {
    glDeleteFramebuffers(1, &fb.m_id);
}

void GL46Framebuffer::attachImage(FramebufferID& fb, FramebufferAttachment attachment, const TextureID& te, int level) const {
    GLenum attachment_gl;
    switch (attachment) {
    case FramebufferAttachment::DEPTH: attachment_gl = GL_DEPTH_ATTACHMENT; break;
    case FramebufferAttachment::STENCIL: attachment_gl = GL_STENCIL_ATTACHMENT; break;
    case FramebufferAttachment::DEPTH_AND_STENCIL: attachment_gl = GL_DEPTH_STENCIL_ATTACHMENT; break;
    default: attachment_gl = GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(attachment); break;
    }
    glNamedFramebufferTexture(fb.m_id, attachment_gl, te.m_id, level);
}

void GL46Framebuffer::associateAttachementsWithOutputs(FramebufferID& fb, const std::vector<FramebufferOutput>& outputs) const {
    std::vector<GLenum> outputs_gl;
    outputs_gl.resize(outputs.size(), GL_NONE);//Expensive malloc...
    for (size_t i = 0; i < outputs.size(); i++) {
        if (outputs[i] == FramebufferOutput::DISCARD) {
            outputs_gl[i] = GL_NONE;
        } else {
            outputs_gl[i] = GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(outputs[i]) - static_cast<GLenum>(FramebufferOutput::TO_COLOR0);
        }
    }
    glNamedFramebufferDrawBuffers(fb.m_id, static_cast<GLsizei>(outputs_gl.size()), outputs_gl.data());
}

void GL46Framebuffer::selectAttachmentForColorReading(FramebufferID& fb, unsigned int colorAttachmentIndex) const {
    glNamedFramebufferReadBuffer(fb.m_id, GL_COLOR_ATTACHMENT0 + colorAttachmentIndex);
}

void GL46Framebuffer::targetMe(const FramebufferID& fb, FramebufferTarget target) const {
    glBindFramebuffer(convert(target), fb.m_id);
}

FramebufferTargetability GL46Framebuffer::checkTargetability(const FramebufferID& fb, FramebufferTarget target) const {
    switch (glCheckNamedFramebufferStatus(fb.m_id, convert(target))) {
    case GL_FRAMEBUFFER_COMPLETE: return FramebufferTargetability::TARGETABLE;
    default: return FramebufferTargetability::NOT_COMPLETE;
    }
}

void GL46Framebuffer::clearColorAttachment(const FramebufferID& fb, unsigned int attachmentIndex, const glm::vec4& color) const {
    glClearNamedFramebufferfv(fb.m_id, GL_COLOR, attachmentIndex, const_cast<float*>(&color.r));
}

void GL46Framebuffer::clearColorAttachment(const FramebufferID& fb, unsigned int attachmentIndex, const glm::ivec4& color) const {
    glClearNamedFramebufferiv(fb.m_id, GL_COLOR, attachmentIndex, &color.r);
}

void GL46Framebuffer::clearColorAttachment(const FramebufferID& fb, unsigned int attachmentIndex, const glm::uvec4& color) const {
    glClearNamedFramebufferuiv(fb.m_id, GL_COLOR, attachmentIndex, &color.r);
}

void GL46Framebuffer::clearDepthAttachment(const FramebufferID& fb, float depth) const {
    glClearNamedFramebufferfv(fb.m_id, GL_DEPTH, 0, &depth);
}

void GL46Framebuffer::clearStencilAttachment(const FramebufferID& fb, int stencil) const {
    glClearNamedFramebufferiv(fb.m_id, GL_STENCIL, 0, &stencil);
}

void GL46Framebuffer::clearDepthAndStencilAttachments(const FramebufferID& fb, float depth, int stencil) const {
    glClearNamedFramebufferfi(fb.m_id, GL_DEPTH_STENCIL, 0, depth, stencil);
}

}