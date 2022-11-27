﻿/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/output/Framebuffer.hpp>

#include <RealEngine/rendering/textures/Texture.hpp>

#include <RealEngine/rendering/internal_renderers/VK13Framebuffer.hpp>


namespace RE {

template<Renderer R>
Framebuffer<R>::Framebuffer() :
    m_id(s_impl->construct()) {
}

template<Renderer R>
Framebuffer<R>::Framebuffer(Framebuffer<R>&& other) noexcept :
    m_id(std::move(other.m_id)) {
}

template<Renderer R>
Framebuffer<R>& Framebuffer<R>::operator=(Framebuffer<R>&& other) noexcept {
    m_id = std::move(other.m_id);
    return *this;
}

template<Renderer R>
Framebuffer<R>::~Framebuffer() {
    s_impl->destruct(m_id);
}

template<Renderer R>
void Framebuffer<R>::attachImage(FramebufferAttachment attachment, const Texture<R>& te, int level) {
    s_impl->attachImage(m_id, attachment, te.m_id, level);
}

template<Renderer R>
void Framebuffer<R>::associateAttachementsWithOutputs(const std::vector<FramebufferOutput>& outputs) {
    s_impl->associateAttachementsWithOutputs(m_id, outputs);
}

template<Renderer R>
void Framebuffer<R>::selectAttachmentForColorReading(unsigned int colorAttachmentIndex) {
    s_impl->selectAttachmentForColorReading(m_id, colorAttachmentIndex);
}

template<Renderer R>
void Framebuffer<R>::targetMe(FramebufferTarget target) const {
    s_impl->targetMe(m_id, target);
}

template<Renderer R>
Framebuffer<R>::Framebuffer(FramebufferID&& id) :
    m_id(std::move(id)) {
}

template<Renderer R>
FramebufferTargetability Framebuffer<R>::checkTargetability(FramebufferTarget target) const {
    return s_impl->checkTargetability(m_id, target);
}

template<Renderer R>
void Framebuffer<R>::clearColorAttachment(unsigned int attachmentIndex, const glm::vec4& color) const {
    s_impl->clearColorAttachment(m_id, attachmentIndex, color);
}

template<Renderer R>
void Framebuffer<R>::clearColorAttachment(unsigned int attachmentIndex, const glm::ivec4& color) const {
    s_impl->clearColorAttachment(m_id, attachmentIndex, color);
}

template<Renderer R>
void Framebuffer<R>::clearColorAttachment(unsigned int attachmentIndex, const glm::uvec4& color) const {
    s_impl->clearColorAttachment(m_id, attachmentIndex, color);
}

template<Renderer R>
void Framebuffer<R>::clearDepthAttachment(float depth) const {
    s_impl->clearDepthAttachment(m_id, depth);
}

template<Renderer R>
void Framebuffer<R>::clearStencilAttachment(int stencil) const {
    s_impl->clearStencilAttachment(m_id, stencil);
}

template<Renderer R>
void Framebuffer<R>::clearDepthAndStencilAttachments(float depth, int stencil) const {
    s_impl->clearDepthAndStencilAttachments(m_id, depth, stencil);
}

template<Renderer R>
void DefaultFrameBuffer<R>::targetMe(FramebufferTarget target) {
    s_defaultFramebuffer->targetMe(target);
}

template class Framebuffer<RendererLateBind>;
template class Framebuffer<RendererVK13>;

template class DefaultFrameBuffer<RendererLateBind>;
template class DefaultFrameBuffer<RendererVK13>;

}