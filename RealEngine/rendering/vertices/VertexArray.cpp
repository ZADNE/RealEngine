/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/vertices/VertexArray.hpp>

#include <RealEngine/rendering/internal_renderers/GL46VertexArray.hpp>
#include <RealEngine/rendering/internal_renderers/VK13VertexArray.hpp>

namespace RE {

template<Renderer R>
VertexArray<R>::VertexArray() :
    m_id(s_impl->construct()) {
}

template<Renderer R>
VertexArray<R>::~VertexArray() {
    s_impl->destruct(m_id);
}

template<Renderer R>
VertexArray<R>::VertexArray(VertexArray<R>&& other) noexcept :
    m_id(std::move(other.m_id)) {
}

template<Renderer R>
VertexArray<R>& VertexArray<R>::operator=(VertexArray<R>&& other) noexcept {
    m_id = std::move(other.m_id);
    return *this;
}

template<Renderer R>
void VertexArray<R>::setAttribute(unsigned int attribute, VertexComponentCount components, VertexComponentType type, unsigned int relativeOffset, bool normalize/* = true*/) {
    s_impl->setAttribute(m_id, attribute, components, type, relativeOffset, normalize);
}

template<Renderer R>
void VertexArray<R>::unsetAttribute(unsigned int attribute) {
    s_impl->unsetAttribute(m_id, attribute);
}

template<Renderer R>
void VertexArray<R>::setBindingPoint(unsigned int bindingPoint, const Buffer<R>& buffer, int offset, int stride) {
    s_impl->setBindingPoint(m_id, bindingPoint, buffer.m_id, offset, stride);
}

template<Renderer R>
void VertexArray<R>::unsetBindingPoint(unsigned int bindingPoint) {
    s_impl->unsetBindingPoint(m_id, bindingPoint);
}

template<Renderer R>
void VertexArray<R>::connectAttributeToBindingPoint(unsigned int attribute, unsigned int bindingPoint) {
    s_impl->connectAttributeToBindingPoint(m_id, attribute, bindingPoint);
}

template<Renderer R>
void VertexArray<R>::bind() const {
    s_impl->bind(m_id);
}

template<Renderer R>
void VertexArray<R>::unbind() const {
    s_impl->unbind(m_id);
}

template<Renderer R>
void VertexArray<R>::renderArrays(Primitive prim, int first, size_t count) const {
    s_impl->renderArrays(m_id, prim, first, count);
}

template<Renderer R>
void VertexArray<R>::renderArrays(Primitive prim, int first, size_t count, int instancecount) const {
    s_impl->renderArrays(m_id, prim, first, count, instancecount);
}

template<Renderer R>
void VertexArray<R>::renderElements(Primitive prim, size_t count, IndexType type, const void* indices) const {
    s_impl->renderElements(m_id, prim, count, type, indices);
}

template<Renderer R>
void VertexArray<R>::renderElements(Primitive prim, size_t count, IndexType type, const void* indices, int instancecount) const {
    s_impl->renderElements(m_id, prim, count, type, indices, instancecount);
}

template<Renderer R>
void VertexArray<R>::renderElementsBaseVertex(Primitive prim, size_t count, IndexType type, const void* indices, int basevertex) const {
    s_impl->renderElementsBaseVertex(m_id, prim, count, type, indices, basevertex);
}

template<Renderer R>
void VertexArray<R>::renderElementsBaseVertex(Primitive prim, size_t count, IndexType type, const void* indices, int instancecount, int basevertex) const {
    s_impl->renderElementsBaseVertex(m_id, prim, count, type, indices, instancecount, basevertex);
}

template VertexArray<RendererLateBind>;
template VertexArray<RendererVK13>;
template VertexArray<RendererGL46>;

}