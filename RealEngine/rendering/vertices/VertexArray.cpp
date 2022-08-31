/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/vertices/VertexArray.hpp>

#include <RealEngine/rendering/RendererLateBind.hpp>
#include <RealEngine/rendering/RendererGL46.hpp>

namespace RE {

template<typename R>
VertexArray<R>::VertexArray() :
    m_internals(s_impl->construct()) {
}

template<typename R>
VertexArray<R>::~VertexArray() {
    s_impl->destruct(m_internals);
}

template<typename R>
VertexArray<R>::VertexArray(VertexArray<R>&& other) noexcept :
    m_internals(std::move(other.m_internals)) {
}

template<typename R>
VertexArray<R>& VertexArray<R>::operator=(VertexArray<R>&& other) noexcept {
    m_internals = std::move(other.m_internals);
    return *this;
}

template<typename R>
void VertexArray<R>::setAttribute(unsigned int attribute, VertexComponentCount components, VertexComponentType type, unsigned int relativeOffset, bool normalize/* = true*/) {
    s_impl->setAttribute(m_internals, attribute, components, type, relativeOffset, normalize);
}

template<typename R>
void VertexArray<R>::unsetAttribute(unsigned int attribute) {
    s_impl->unsetAttribute(m_internals, attribute);
}

template<typename R>
void VertexArray<R>::setBindingPoint(unsigned int bindingPoint, const Buffer<R>& buffer, int offset, int stride) {
    s_impl->setBindingPoint(m_internals, bindingPoint, buffer.m_internals, offset, stride);
}

template<typename R>
void VertexArray<R>::unsetBindingPoint(unsigned int bindingPoint) {
    s_impl->unsetBindingPoint(m_internals, bindingPoint);
}

template<typename R>
void VertexArray<R>::connectAttributeToBindingPoint(unsigned int attribute, unsigned int bindingPoint) {
    s_impl->connectAttributeToBindingPoint(m_internals, attribute, bindingPoint);
}

template<typename R>
void VertexArray<R>::bind() const {
    s_impl->bind(m_internals);
}

template<typename R>
void VertexArray<R>::unbind() const {
    s_impl->unbind(m_internals);
}

template<typename R>
void VertexArray<R>::renderArrays(Primitive prim, int first, size_t count) const {
    s_impl->renderArrays(m_internals, prim, first, count);
}

template<typename R>
void VertexArray<R>::renderArrays(Primitive prim, int first, size_t count, int instancecount) const {
    s_impl->renderArrays(m_internals, prim, first, count, instancecount);
}

template<typename R>
void VertexArray<R>::renderElements(Primitive prim, size_t count, IndexType type, const void* indices) const {
    s_impl->renderElements(m_internals, prim, count, type, indices);
}

template<typename R>
void VertexArray<R>::renderElements(Primitive prim, size_t count, IndexType type, const void* indices, int instancecount) const {
    s_impl->renderElements(m_internals, prim, count, type, indices, instancecount);
}

template<typename R>
void VertexArray<R>::renderElementsBaseVertex(Primitive prim, size_t count, IndexType type, const void* indices, int basevertex) const {
    s_impl->renderElementsBaseVertex(m_internals, prim, count, type, indices, basevertex);
}

template<typename R>
void VertexArray<R>::renderElementsBaseVertex(Primitive prim, size_t count, IndexType type, const void* indices, int instancecount, int basevertex) const {
    s_impl->renderElementsBaseVertex(m_internals, prim, count, type, indices, instancecount, basevertex);
}

template VertexArray<RendererLateBind>;
template VertexArray<RendererGL46>;

}