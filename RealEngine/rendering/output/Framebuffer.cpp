/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/output/Framebuffer.hpp>

#include <cassert>


namespace RE {

IFramebuffer* Framebuffer::s_impl = nullptr;

Framebuffer::Framebuffer() {
    s_impl->construct(*this);
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept :
    m_ID(other.m_ID) {
    other.m_ID = 0;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
    std::swap(m_ID, other.m_ID);
    return *this;
}

Framebuffer::~Framebuffer() {
    s_impl->destruct(*this);
}

void Framebuffer::attachImage(FramebufferAttachment attachment, const Texture& te, int level) {
    s_impl->attachImage(*this, attachment, te, level);
}

void Framebuffer::associateAttachementsWithOutputs(const std::vector<FramebufferOutput>& outputs) {
    s_impl->associateAttachementsWithOutputs(*this, outputs);
}

void Framebuffer::selectAttachmentForColorReading(unsigned int colorAttachmentIndex) {
    s_impl->selectAttachmentForColorReading(*this, colorAttachmentIndex);
}

void Framebuffer::targetMe(FramebufferTarget target) const {
    s_impl->targetMe(*this, target);
}

Framebuffer::Framebuffer(unsigned int ID):
    m_ID(ID) {
    assert(m_ID == 0);
}

FramebufferTargetability Framebuffer::checkTargetability(FramebufferTarget target) const {
    return s_impl->checkTargetability(*this, target);
}

void Framebuffer::clearColorAttachment(unsigned int attachmentIndex, const glm::vec4& color) const {
    s_impl->clearColorAttachment(*this, attachmentIndex, color);
}

void Framebuffer::clearColorAttachment(unsigned int attachmentIndex, const glm::ivec4& color) const {
    s_impl->clearColorAttachment(*this, attachmentIndex, color);
}

void Framebuffer::clearColorAttachment(unsigned int attachmentIndex, const glm::uvec4& color) const {
    s_impl->clearColorAttachment(*this, attachmentIndex, color);
}

void Framebuffer::clearDepthAttachment(float depth) const {
    s_impl->clearDepthAttachment(*this, depth);
}

void Framebuffer::clearStencilAttachment(int stencil) const {
    s_impl->clearStencilAttachment(*this, stencil);
}

void Framebuffer::clearDepthAndStencilAttachments(float depth, int stencil) const {
    s_impl->clearDepthAndStencilAttachments(*this, depth, stencil);
}

void DefaultFrameBuffer::targetMe(FramebufferTarget target) {
    s_defaultFramebuffer->targetMe(target);
}

void DefaultFrameBuffer::clearColor(const glm::vec4& color) {
    s_defaultFramebuffer->clearColorAttachment(0u, color);
}

void DefaultFrameBuffer::clearColor(const glm::ivec4& color) {
    s_defaultFramebuffer->clearColorAttachment(0u, color);
}

void DefaultFrameBuffer::clearColor(const glm::uvec4& color) {
    s_defaultFramebuffer->clearColorAttachment(0u, color);
}

void DefaultFrameBuffer::clearDepth(float depth) {
    s_defaultFramebuffer->clearDepthAttachment(depth);
}

void DefaultFrameBuffer::clearStencil(int stencil) {
    s_defaultFramebuffer->clearStencilAttachment(stencil);
}

void DefaultFrameBuffer::clearDepthAndStencil(float depth, int stencil) {
    s_defaultFramebuffer->clearDepthAndStencilAttachments(depth, stencil);
}

Framebuffer* DefaultFrameBuffer::s_defaultFramebuffer = nullptr;

}