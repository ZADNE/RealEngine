﻿/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/GL46_VertexArray.hpp>

#include <RealEngine/rendering/vertices/VertexArray.hpp>

namespace RE {

void GL46_VertexArray::construct(VertexArray& va) const {
	glCreateVertexArrays(1, &va.m_ID);
}

void GL46_VertexArray::destruct(VertexArray& va) const {
	glDeleteVertexArrays(1, &va.m_ID);
}

void GL46_VertexArray::setAttribute(VertexArray& va, GLuint attribute, VertexComponentCount components, VertexComponentType type, GLuint relativeOffset, bool normalize/* = true*/) const {
	glEnableVertexArrayAttrib(va.m_ID, attribute);

	switch (type) {
	case RE::VertexComponentType::BYTE:
	case RE::VertexComponentType::SHORT:
	case RE::VertexComponentType::INT:
	case RE::VertexComponentType::UNSIGNED_BYTE:
	case RE::VertexComponentType::UNSIGNED_SHORT:
	case RE::VertexComponentType::UNSIGNED_INT:
	case RE::VertexComponentType::FIXED:
		if (normalize) {
			glVertexArrayAttribFormat(va.m_ID, attribute, static_cast<GLint>(components), static_cast<GLenum>(type), GL_TRUE, relativeOffset);
		} else {
			glVertexArrayAttribIFormat(va.m_ID, attribute, static_cast<GLint>(components), static_cast<GLenum>(type), relativeOffset);
		}
		break;
	case RE::VertexComponentType::HALF_FLOAT:
	case RE::VertexComponentType::FLOAT:
		glVertexArrayAttribFormat(va.m_ID, attribute, static_cast<GLint>(components), static_cast<GLenum>(type), GL_FALSE, relativeOffset);
		break;
	case RE::VertexComponentType::DOUBLE:
		glVertexArrayAttribLFormat(va.m_ID, attribute, static_cast<GLint>(components), static_cast<GLenum>(type), relativeOffset);
		break;
	default:
		break;
	}
}

void GL46_VertexArray::unsetAttribute(VertexArray& va, GLuint attribute) const {
	glDisableVertexArrayAttrib(va.m_ID, attribute);
}

void GL46_VertexArray::setBindingPoint(VertexArray& va, GLuint bindingPoint, const Buffer& buffer, GLintptr offset, GLsizei stride) const {
	glVertexArrayVertexBuffer(va.m_ID, bindingPoint, buffer.m_ID, offset, stride);
}

void GL46_VertexArray::unsetBindingPoint(VertexArray& va, GLuint bindingPoint) const {
	glVertexArrayVertexBuffer(va.m_ID, bindingPoint, 0, 0, 0);
}

void GL46_VertexArray::connectAttributeToBindingPoint(VertexArray& va, GLuint attribute, GLuint bindingPoint) const {
	glVertexArrayAttribBinding(va.m_ID, attribute, bindingPoint);
}

void GL46_VertexArray::bind(const VertexArray& va) const {
#ifdef _DEBUG
	if (s_currentlyBoundID != 0) {
		throw "Overbound vertex arrays";
	}
	s_currentlyBoundID = va.m_ID;
#endif // _DEBUG
	glBindVertexArray(va.m_ID);
}

void GL46_VertexArray::unbind(const VertexArray& va) const {
#ifdef _DEBUG
	if (s_currentlyBoundID != va.m_ID) {
		throw "Overbound vertex arrays";
	}
	s_currentlyBoundID = 0;
	glBindVertexArray(0);
#endif // _DEBUG
}

void GL46_VertexArray::renderArrays(const VertexArray& va, Primitive prim, GLint first, GLsizei count) const {
	throwIfNotCurrentlyBound(va);
	glDrawArrays(static_cast<GLenum>(prim), first, count);
}

void GL46_VertexArray::renderArrays(const VertexArray& va, Primitive prim, GLint first, GLsizei count, GLsizei instancecount) const {
	throwIfNotCurrentlyBound(va);
	glDrawArraysInstanced(static_cast<GLenum>(prim), first, count, instancecount);
}

void GL46_VertexArray::renderElements(const VertexArray& va, Primitive prim, GLsizei count, IndexType type, const void* indices) const {
	throwIfNotCurrentlyBound(va);
	glDrawElements(static_cast<GLenum>(prim), count, static_cast<GLenum>(type), indices);
}

void GL46_VertexArray::renderElements(const VertexArray& va, Primitive prim, GLsizei count, IndexType type, const void* indices, GLsizei instancecount) const {
	throwIfNotCurrentlyBound(va);
	glDrawElementsInstanced(static_cast<GLenum>(prim), count, static_cast<GLenum>(type), indices, instancecount);
}

void GL46_VertexArray::renderElementsBaseVertex(const VertexArray& va, Primitive prim, GLsizei count, IndexType type, const void* indices, GLint basevertex) const {
	throwIfNotCurrentlyBound(va);
	glDrawElementsBaseVertex(static_cast<GLenum>(prim), count, static_cast<GLenum>(type), const_cast<void*>(indices), basevertex);
}

void GL46_VertexArray::renderElementsBaseVertex(const VertexArray& va, Primitive prim, GLsizei count, IndexType type, const void* indices, GLsizei instancecount, GLint basevertex) const {
	throwIfNotCurrentlyBound(va);
	glDrawElementsInstancedBaseVertex(static_cast<GLenum>(prim), count, static_cast<GLenum>(type), indices, instancecount, basevertex);
}

void GL46_VertexArray::throwIfNotCurrentlyBound(const VertexArray& va) const {
#ifdef _DEBUG
	if (s_currentlyBoundID != va.m_ID) {
		throw "Rendered with vertex array that is not bound";
	}
#endif // _DEBUG
}

}