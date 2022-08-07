/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/VertexArray.hpp>

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

void VertexArray::setAttribute(GLuint attribute, VertexComponentCount components, VertexComponentType type, GLuint relativeOffset, bool normalize/* = true*/) {
	s_impl->setAttribute(*this, attribute, components, type, relativeOffset, normalize);
}

void VertexArray::unsetAttribute(GLuint attribute) {
	s_impl->unsetAttribute(*this, attribute);
}

void VertexArray::setBindingPoint(GLuint bindingPoint, const Buffer& buffer, GLintptr offset, GLsizei stride) {
	s_impl->setBindingPoint(*this, bindingPoint, buffer, offset, stride);
}

void VertexArray::unsetBindingPoint(GLuint bindingPoint) {
	s_impl->unsetBindingPoint(*this, bindingPoint);
}

void VertexArray::connectAttributeToBindingPoint(GLuint attribute, GLuint bindingPoint) {
	s_impl->connectAttributeToBindingPoint(*this, attribute, bindingPoint);
}

void VertexArray::bind() const {
	s_impl->bind(*this);
}

void VertexArray::unbind() const {
	s_impl->unbind(*this);
}

void VertexArray::renderArrays(Primitive prim, GLint first, GLsizei count) const {
	s_impl->renderArrays(*this, prim, first, count);
}

void VertexArray::renderArrays(Primitive prim, GLint first, GLsizei count, GLsizei instancecount) const {
	s_impl->renderArrays(*this, prim, first, count, instancecount);
}

void VertexArray::renderElements(Primitive prim, GLsizei count, IndexType type, const void* offset) const {
	s_impl->renderElements(*this, prim, count, type, offset);
}

void VertexArray::renderElements(Primitive prim, GLsizei count, IndexType type, const void* offset, GLsizei instancecount) const {
	s_impl->renderElements(*this, prim, count, type, offset, instancecount);
}

}