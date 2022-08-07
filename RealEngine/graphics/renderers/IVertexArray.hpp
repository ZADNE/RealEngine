/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <GL/glew.h>

#include <glm/vec2.hpp>

#include <RealEngine/graphics/buffers/Buffer.hpp>

namespace RE {

enum class VertexComponentCount : GLint {
	ONE = 1,
	TWO = 2,
	THREE = 3,
	FOUR = 4,

	X = ONE,
	XY = TWO,
	XYZ = THREE,
	XYZW = FOUR,

	R = ONE,
	RG = TWO,
	RGB = THREE,
	RGBA = FOUR
};

enum class VertexComponentType : GLenum {
	BYTE = GL_BYTE,
	SHORT = GL_SHORT,
	INT = GL_INT,
	UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
	UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
	UNSIGNED_INT = GL_UNSIGNED_INT,
	FIXED = GL_FIXED,
	HALF_FLOAT = GL_HALF_FLOAT,
	FLOAT = GL_FLOAT,
	DOUBLE = GL_DOUBLE
};

enum class Primitive : GLenum {
	POINTS = GL_POINTS,
	LINE_STRIP = GL_LINE_STRIP,
	LINE_LOOP = GL_LINE_LOOP,
	LINES = GL_LINES,
	LINE_STRIP_ADJACENCY = GL_LINE_STRIP_ADJACENCY,
	LINES_ADJACENCY = GL_LINES_ADJACENCY,
	TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
	TRIANGLE_FAN = GL_TRIANGLE_FAN,
	TRIANGLES = GL_TRIANGLES,
	TRIANGLE_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY,
	TRIANGLES_ADJACENCY = GL_TRIANGLES_ADJACENCY,
	PATCHES = GL_PATCHES
};

enum class IndexType : GLenum {
	UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
	UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
	UNSIGNED_INT = GL_UNSIGNED_INT
};

class VertexArray;

class IVertexArray {
public:
	virtual void construct(VertexArray& va) const = 0;
	virtual void destruct(VertexArray& va) const = 0;

	virtual void setAttribute(VertexArray& va, GLuint attribute, VertexComponentCount components, VertexComponentType type, GLuint relativeOffset, bool normalize = true) const = 0;
	virtual void unsetAttribute(VertexArray& va, GLuint attribute) const = 0;

	virtual void setBindingPoint(VertexArray& va, GLuint bindingPoint, const Buffer& buffer, GLintptr offset, GLsizei stride) const = 0;
	virtual void unsetBindingPoint(VertexArray& va, GLuint bindingPoint) const = 0;

	virtual void connectAttributeToBindingPoint(VertexArray& va, GLuint attribute, GLuint bindingPoint) const = 0;

	virtual void bind(const VertexArray& va) const = 0;
	virtual void unbind(const VertexArray& va) const = 0;

	virtual void renderArrays(const VertexArray& va, Primitive prim, GLint first, GLsizei count) const = 0;
	virtual void renderArrays(const VertexArray& va, Primitive prim, GLint first, GLsizei count, GLsizei instancecount) const = 0;

	virtual void renderElements(const VertexArray& va, Primitive prim, GLsizei count, IndexType type, const void* offset) const = 0;
	virtual void renderElements(const VertexArray& va, Primitive prim, GLsizei count, IndexType type, const void* offset, GLsizei instancecount) const = 0;
};

}