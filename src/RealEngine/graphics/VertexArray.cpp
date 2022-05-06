/*! 
 *  \author    Dubsky Tomas
 */
#include <RealEngine/graphics/VertexArray.hpp>

namespace RE {

VertexArray::VertexArray() {
	glCreateVertexArrays(1, &m_ID);
}

VertexArray::~VertexArray() {
	glDeleteVertexArrays(1, &m_ID);
}

void VertexArray::setAttribute(GLuint attribute, VertexComponentCount components, VertexComponentType type, GLuint relativeOffset, bool normalize/* = true*/) {
	glEnableVertexArrayAttrib(m_ID, attribute);

	switch (type) {
	case RE::VertexComponentType::BYTE:
	case RE::VertexComponentType::SHORT:
	case RE::VertexComponentType::INT:
	case RE::VertexComponentType::UNSIGNED_BYTE:
	case RE::VertexComponentType::UNSIGNED_SHORT:
	case RE::VertexComponentType::UNSIGNED_INT:
	case RE::VertexComponentType::FIXED:
		if (normalize) {
			glVertexArrayAttribFormat(m_ID, attribute, static_cast<GLint>(components), static_cast<GLenum>(type), GL_TRUE, relativeOffset);
		} else {
			glVertexArrayAttribIFormat(m_ID, attribute, static_cast<GLint>(components), static_cast<GLenum>(type), relativeOffset);
		}
		break;
	case RE::VertexComponentType::HALF_FLOAT:
	case RE::VertexComponentType::FLOAT:
		glVertexArrayAttribFormat(m_ID, attribute, static_cast<GLint>(components), static_cast<GLenum>(type), GL_FALSE, relativeOffset);
		break;
	case RE::VertexComponentType::DOUBLE:
		glVertexArrayAttribLFormat(m_ID, attribute, static_cast<GLint>(components), static_cast<GLenum>(type), relativeOffset);
		break;
	default:
		break;
	}
}

void VertexArray::unsetAttribute(GLuint attribute) {
	glDisableVertexArrayAttrib(m_ID, attribute);
}

void VertexArray::unsetBindingPoint(GLuint bindingPoint) {
	glVertexArrayVertexBuffer(m_ID, bindingPoint, 0, 0, 0);
}

void VertexArray::connectAttributeToBindingPoint(GLuint attribute, GLuint bindingPoint) {
	glVertexArrayAttribBinding(m_ID, attribute, bindingPoint);
}

void VertexArray::bind() const {
#ifdef _DEBUG
	if (m_currentlyBoundID != 0) {
		throw "Overbound vertex arrays";
	}
	m_currentlyBoundID = m_ID;
#endif // _DEBUG
	glBindVertexArray(m_ID);
}

void VertexArray::unbind() const {
#ifdef _DEBUG
	if (m_currentlyBoundID != m_ID) {
		throw "Overbound vertex arrays";
	}
	m_currentlyBoundID = 0;
	glBindVertexArray(0);
#endif // _DEBUG
}

void VertexArray::renderArrays(Primitive prim, GLint first, GLsizei count) {
	throwIfNotCurrentlyBound();
	glDrawArrays(static_cast<GLenum>(prim), first, count);
}

void VertexArray::renderArrays(Primitive prim, GLint first, GLsizei count, GLsizei instancecount) {
	throwIfNotCurrentlyBound();
	glDrawArraysInstanced(static_cast<GLenum>(prim), first, count, instancecount);
}

void VertexArray::renderElements(Primitive prim, GLsizei count, IndexType type, const void* offset) {
	throwIfNotCurrentlyBound();
	glDrawElements(static_cast<GLenum>(prim), count, static_cast<GLenum>(type), offset);
}

void VertexArray::renderElements(Primitive prim, GLsizei count, IndexType type, const void* offset, GLsizei instancecount) {
	throwIfNotCurrentlyBound();
	glDrawElementsInstanced(static_cast<GLenum>(prim), count, static_cast<GLenum>(type), offset, instancecount);
}

void VertexArray::throwIfNotCurrentlyBound() {
#ifdef _DEBUG
	if (m_currentlyBoundID != m_ID) {
		throw "Rendered with vertex array that is not bound";
	}
#endif // _DEBUG
}


}