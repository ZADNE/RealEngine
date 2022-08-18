/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <glm/vec2.hpp>

#include <RealEngine/rendering/buffers/Buffer.hpp>

namespace RE {

enum class VertexComponentCount : int {
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

enum class VertexComponentType : unsigned int {
	BYTE,
	SHORT,
	INT,
	UNSIGNED_BYTE,
	UNSIGNED_SHORT,
	UNSIGNED_INT,
	FIXED,
	HALF_FLOAT,
	FLOAT,
	DOUBLE
};

enum class Primitive : unsigned int {
	POINTS,
	LINE_STRIP,
	LINE_LOOP,
	LINES,
	LINE_STRIP_ADJACENCY,
	LINES_ADJACENCY,
	TRIANGLE_STRIP,
	TRIANGLE_FAN,
	TRIANGLES,
	TRIANGLE_STRIP_ADJACENCY,
	TRIANGLES_ADJACENCY,
	PATCHES
};

enum class IndexType : unsigned int {
	UNSIGNED_BYTE,
	UNSIGNED_SHORT,
	UNSIGNED_INT
};

class VertexArray;

/**
* @brief Is a renderer-agnostic interface to VertexArray's implementation.
*
* This is used internally by VertexArray.
*
* @see RE::VertexArray
*/
class IVertexArray {
public:
	virtual void construct(VertexArray& va) const = 0;
	virtual void destruct(VertexArray& va) const = 0;

	virtual void setAttribute(VertexArray& va, unsigned int attribute, VertexComponentCount components, VertexComponentType type, unsigned int relativeOffset, bool normalize = true) const = 0;
	virtual void unsetAttribute(VertexArray& va, unsigned int attribute) const = 0;

	virtual void setBindingPoint(VertexArray& va, unsigned int bindingPoint, const Buffer& buffer, int offset, int stride) const = 0;
	virtual void unsetBindingPoint(VertexArray& va, unsigned int bindingPoint) const = 0;

	virtual void connectAttributeToBindingPoint(VertexArray& va, unsigned int attribute, unsigned int bindingPoint) const = 0;

	virtual void bind(const VertexArray& va) const = 0;
	virtual void unbind(const VertexArray& va) const = 0;

	virtual void renderArrays(const VertexArray& va, Primitive prim, int first, int count) const = 0;
	virtual void renderArrays(const VertexArray& va, Primitive prim, int first, int count, int instancecount) const = 0;

	virtual void renderElements(const VertexArray& va, Primitive prim, int count, IndexType type, const void* offset) const = 0;
	virtual void renderElements(const VertexArray& va, Primitive prim, int count, IndexType type, const void* offset, int instancecount) const = 0;

	virtual void renderElementsBaseVertex(const VertexArray& va, Primitive prim, int count, IndexType type, const void* indices, int basevertex) const = 0;
	virtual void renderElementsBaseVertex(const VertexArray& va, Primitive prim, int count, IndexType type, const void* indices, int instancecount, int basevertex) const = 0;
};

}