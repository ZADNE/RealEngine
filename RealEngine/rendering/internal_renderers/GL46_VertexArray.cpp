/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/GL46_VertexArray.hpp>

#include <GL/glew.h>

#include <RealEngine/utility/error.hpp>
#include <RealEngine/rendering/vertices/VertexArray.hpp>

namespace RE {

GLint convert(VertexComponentCount components) {
	return static_cast<GLint>(components);
}

GLenum convert(VertexComponentType type) {
	switch (type) {
	case VertexComponentType::BYTE: return GL_BYTE;
	case VertexComponentType::SHORT: return GL_SHORT;
	case VertexComponentType::INT: return GL_INT;
	case VertexComponentType::UNSIGNED_BYTE: return GL_UNSIGNED_BYTE;
	case VertexComponentType::UNSIGNED_SHORT: return GL_UNSIGNED_SHORT;
	case VertexComponentType::UNSIGNED_INT: return GL_UNSIGNED_INT;
	case VertexComponentType::FIXED: return GL_FIXED;
	case VertexComponentType::HALF_FLOAT: return GL_HALF_FLOAT;
	case VertexComponentType::FLOAT: return GL_FLOAT;
	case VertexComponentType::DOUBLE: return GL_DOUBLE;
	default: fatalError("Bad enum value of VertexComponentType!");
	}
}

GLenum convert(Primitive primitive) {
	switch (primitive) {
	case Primitive::POINTS: return GL_POINTS;
	case Primitive::LINE_STRIP: return GL_LINE_STRIP;
	case Primitive::LINE_LOOP: return GL_LINE_LOOP;
	case Primitive::LINES: return GL_LINES;
	case Primitive::LINE_STRIP_ADJACENCY: return GL_LINE_STRIP_ADJACENCY;
	case Primitive::LINES_ADJACENCY: return GL_LINES_ADJACENCY;
	case Primitive::TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
	case Primitive::TRIANGLE_FAN: return GL_TRIANGLE_FAN;
	case Primitive::TRIANGLES: return GL_TRIANGLES;
	case Primitive::TRIANGLE_STRIP_ADJACENCY: return GL_TRIANGLE_STRIP_ADJACENCY;
	case Primitive::TRIANGLES_ADJACENCY: return GL_TRIANGLES_ADJACENCY;
	case Primitive::PATCHES: return GL_PATCHES;
	default: fatalError("Bad enum value of Primitive!");
	}
}

GLenum convert(IndexType type) {
	switch (type) {
	case IndexType::UNSIGNED_BYTE: return GL_UNSIGNED_BYTE;
	case IndexType::UNSIGNED_SHORT: return GL_UNSIGNED_SHORT;
	case IndexType::UNSIGNED_INT: return GL_UNSIGNED_INT;
	default: fatalError("Bad enum value of IndexType!");
	}
}

void GL46_VertexArray::construct(VertexArray& va) const {
	glCreateVertexArrays(1, &va.m_ID);
}

void GL46_VertexArray::destruct(VertexArray& va) const {
	glDeleteVertexArrays(1, &va.m_ID);
}

void GL46_VertexArray::setAttribute(VertexArray& va, unsigned int attribute, VertexComponentCount components, VertexComponentType type, unsigned int relativeOffset, bool normalize/* = true*/) const {
	glEnableVertexArrayAttrib(va.m_ID, attribute);

	switch (type) {
	case VertexComponentType::BYTE:
	case VertexComponentType::SHORT:
	case VertexComponentType::INT:
	case VertexComponentType::UNSIGNED_BYTE:
	case VertexComponentType::UNSIGNED_SHORT:
	case VertexComponentType::UNSIGNED_INT:
	case VertexComponentType::FIXED:
		if (normalize) {
			glVertexArrayAttribFormat(va.m_ID, attribute, convert(components), convert(type), GL_TRUE, relativeOffset);
		} else {
			glVertexArrayAttribIFormat(va.m_ID, attribute, convert(components), convert(type), relativeOffset);
		}
		break;
	case VertexComponentType::HALF_FLOAT:
	case VertexComponentType::FLOAT:
		glVertexArrayAttribFormat(va.m_ID, attribute, convert(components), convert(type), GL_FALSE, relativeOffset);
		break;
	case VertexComponentType::DOUBLE:
		glVertexArrayAttribLFormat(va.m_ID, attribute, convert(components), convert(type), relativeOffset);
		break;
	default:
		break;
	}
}

void GL46_VertexArray::unsetAttribute(VertexArray& va, unsigned int attribute) const {
	glDisableVertexArrayAttrib(va.m_ID, attribute);
}

void GL46_VertexArray::setBindingPoint(VertexArray& va, unsigned int bindingPoint, const Buffer& buffer, int offset, int stride) const {
	glVertexArrayVertexBuffer(va.m_ID, bindingPoint, buffer.m_ID, offset, stride);
}

void GL46_VertexArray::unsetBindingPoint(VertexArray& va, unsigned int bindingPoint) const {
	glVertexArrayVertexBuffer(va.m_ID, bindingPoint, 0, 0, 0);
}

void GL46_VertexArray::connectAttributeToBindingPoint(VertexArray& va, unsigned int attribute, unsigned int bindingPoint) const {
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

void GL46_VertexArray::renderArrays(const VertexArray& va, Primitive prim, int first, GLsizei count) const {
	throwIfNotCurrentlyBound(va);
	glDrawArrays(convert(prim), first, count);
}

void GL46_VertexArray::renderArrays(const VertexArray& va, Primitive prim, int first, GLsizei count, GLsizei instancecount) const {
	throwIfNotCurrentlyBound(va);
	glDrawArraysInstanced(convert(prim), first, count, instancecount);
}

void GL46_VertexArray::renderElements(const VertexArray& va, Primitive prim, GLsizei count, IndexType type, const void* indices) const {
	throwIfNotCurrentlyBound(va);
	glDrawElements(convert(prim), count, convert(type), indices);
}

void GL46_VertexArray::renderElements(const VertexArray& va, Primitive prim, GLsizei count, IndexType type, const void* indices, GLsizei instancecount) const {
	throwIfNotCurrentlyBound(va);
	glDrawElementsInstanced(convert(prim), count, convert(type), indices, instancecount);
}

void GL46_VertexArray::renderElementsBaseVertex(const VertexArray& va, Primitive prim, GLsizei count, IndexType type, const void* indices, int basevertex) const {
	throwIfNotCurrentlyBound(va);
	glDrawElementsBaseVertex(convert(prim), count, convert(type), const_cast<void*>(indices), basevertex);
}

void GL46_VertexArray::renderElementsBaseVertex(const VertexArray& va, Primitive prim, GLsizei count, IndexType type, const void* indices, GLsizei instancecount, int basevertex) const {
	throwIfNotCurrentlyBound(va);
	glDrawElementsInstancedBaseVertex(convert(prim), count, convert(type), indices, instancecount, basevertex);
}

void GL46_VertexArray::throwIfNotCurrentlyBound(const VertexArray& va) const {
#ifdef _DEBUG
	if (s_currentlyBoundID != va.m_ID) {
		throw "Rendered with vertex array that is not bound";
	}
#endif // _DEBUG
}

}