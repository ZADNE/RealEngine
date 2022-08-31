/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/output/Framebuffer.hpp>

#include <cassert>

#include <RealEngine/rendering/RendererLateBind.hpp>
#include <RealEngine/rendering/RendererGL46.hpp>


namespace RE {

template<typename R>
Framebuffer<R>::Framebuffer() :
    m_internals(s_impl->construct()) {
}

template<typename R>
Framebuffer<R>::Framebuffer(Framebuffer<R>&& other) noexcept :
    m_internals(std::move(other.m_internals)) {
}

template<typename R>
Framebuffer<R>& Framebuffer<R>::operator=(Framebuffer<R>&& other) noexcept {
    m_internals = std::move(other.m_internals);
    return *this;
}

template<typename R>
Framebuffer<R>::~Framebuffer() {
    s_impl->destruct(m_internals);
}

template<typename R>
void Framebuffer<R>::attachImage(FramebufferAttachment attachment, const Texture& te, int level) {
    s_impl->attachImage(m_internals, attachment, te, level);
}

template<typename R>
void Framebuffer<R>::associateAttachementsWithOutputs(const std::vector<FramebufferOutput>& outputs) {
    s_impl->associateAttachementsWithOutputs(m_internals, outputs);
}

template<typename R>
void Framebuffer<R>::selectAttachmentForColorReading(unsigned int colorAttachmentIndex) {
    s_impl->selectAttachmentForColorReading(m_internals, colorAttachmentIndex);
}

template<typename R>
void Framebuffer<R>::targetMe(FramebufferTarget target) const {
    s_impl->targetMe(m_internals, target);
}

template<typename R>
Framebuffer<R>::Framebuffer(FramebufferInternals&& internals) :
    m_internals(std::move(internals)) {
}

template<typename R>
FramebufferTargetability Framebuffer<R>::checkTargetability(FramebufferTarget target) const {
    return s_impl->checkTargetability(m_internals, target);
}

template<typename R>
void Framebuffer<R>::clearColorAttachment(unsigned int attachmentIndex, const glm::vec4& color) const {
    s_impl->clearColorAttachment(m_internals, attachmentIndex, color);
}

template<typename R>
void Framebuffer<R>::clearColorAttachment(unsigned int attachmentIndex, const glm::ivec4& color) const {
    s_impl->clearColorAttachment(m_internals, attachmentIndex, color);
}

template<typename R>
void Framebuffer<R>::clearColorAttachment(unsigned int attachmentIndex, const glm::uvec4& color) const {
    s_impl->clearColorAttachment(m_internals, attachmentIndex, color);
}

template<typename R>
void Framebuffer<R>::clearDepthAttachment(float depth) const {
    s_impl->clearDepthAttachment(m_internals, depth);
}

template<typename R>
void Framebuffer<R>::clearStencilAttachment(int stencil) const {
    s_impl->clearStencilAttachment(m_internals, stencil);
}

template<typename R>
void Framebuffer<R>::clearDepthAndStencilAttachments(float depth, int stencil) const {
    s_impl->clearDepthAndStencilAttachments(m_internals, depth, stencil);
}

template<typename R>
void DefaultFrameBuffer<R>::targetMe(FramebufferTarget target) {
    s_defaultFramebuffer->targetMe(target);
}

template<typename R>
void DefaultFrameBuffer<R>::clearColor(const glm::vec4& color) {
    s_defaultFramebuffer->clearColorAttachment(0u, color);
}

template<typename R>
void DefaultFrameBuffer<R>::clearColor(const glm::ivec4& color) {
    s_defaultFramebuffer->clearColorAttachment(0u, color);
}

template<typename R>
void DefaultFrameBuffer<R>::clearColor(const glm::uvec4& color) {
    s_defaultFramebuffer->clearColorAttachment(0u, color);
}

template<typename R>
void DefaultFrameBuffer<R>::clearDepth(float depth) {
    s_defaultFramebuffer->clearDepthAttachment(depth);
}

template<typename R>
void DefaultFrameBuffer<R>::clearStencil(int stencil) {
    s_defaultFramebuffer->clearStencilAttachment(stencil);
}

template<typename R>
void DefaultFrameBuffer<R>::clearDepthAndStencil(float depth, int stencil) {
    s_defaultFramebuffer->clearDepthAndStencilAttachments(depth, stencil);
}

template Framebuffer<RendererLateBind>;
template Framebuffer<RendererGL46>;

template DefaultFrameBuffer<RendererLateBind>;
template DefaultFrameBuffer<RendererGL46>;

}