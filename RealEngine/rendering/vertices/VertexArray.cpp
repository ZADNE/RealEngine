/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/vertices/VertexArray.hpp>

namespace RE {

IVertexArray* VertexArray::s_impl = nullptr;

VertexArray::VertexArray() {
    s_impl->construct(*this);
}

VertexArray::~VertexArray() {
    s_impl->destruct(*this);
}

VertexArray::VertexArray(VertexArray&& other) noexcept :
    m_ID(other.m_ID) {
    other.m_ID = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
    std::swap(m_ID, other.m_ID);
    return *this;
}

void VertexArray::setAttribute(unsigned int attribute, VertexComponentCount components, VertexComponentType type, unsigned int relativeOffset, bool normalize/* = true*/) {
    s_impl->setAttribute(*this, attribute, components, type, relativeOffset, normalize);
}

void VertexArray::unsetAttribute(unsigned int attribute) {
    s_impl->unsetAttribute(*this, attribute);
}

void VertexArray::setBindingPoint(unsigned int bindingPoint, const Buffer& buffer, int offset, int stride) {
    s_impl->setBindingPoint(*this, bindingPoint, buffer, offset, stride);
}

void VertexArray::unsetBindingPoint(unsigned int bindingPoint) {
    s_impl->unsetBindingPoint(*this, bindingPoint);
}

void VertexArray::connectAttributeToBindingPoint(unsigned int attribute, unsigned int bindingPoint) {
    s_impl->connectAttributeToBindingPoint(*this, attribute, bindingPoint);
}

void VertexArray::bind() const {
    s_impl->bind(*this);
}

void VertexArray::unbind() const {
    s_impl->unbind(*this);
}

void VertexArray::renderArrays(Primitive prim, int first, size_t count) const {
    s_impl->renderArrays(*this, prim, first, count);
}

void VertexArray::renderArrays(Primitive prim, int first, size_t count, int instancecount) const {
    s_impl->renderArrays(*this, prim, first, count, instancecount);
}

void VertexArray::renderElements(Primitive prim, size_t count, IndexType type, const void* indices) const {
    s_impl->renderElements(*this, prim, count, type, indices);
}

void VertexArray::renderElements(Primitive prim, size_t count, IndexType type, const void* indices, int instancecount) const {
    s_impl->renderElements(*this, prim, count, type, indices, instancecount);
}

void VertexArray::renderElementsBaseVertex(Primitive prim, size_t count, IndexType type, const void* indices, int basevertex) const {
    s_impl->renderElementsBaseVertex(*this, prim, count, type, indices, basevertex);
}

void VertexArray::renderElementsBaseVertex(Primitive prim, size_t count, IndexType type, const void* indices, int instancecount, int basevertex) const {
    s_impl->renderElementsBaseVertex(*this, prim, count, type, indices, instancecount, basevertex);
}

}