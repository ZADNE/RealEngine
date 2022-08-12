﻿/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IVertexArray.hpp>

namespace RE {

/**
* @brief Is OpenGL 4.6 implementation of VertexArray class.
*
* Do not use this directly - use VertexArray instead.
*/
class GL46_VertexArray : public IVertexArray {
public:

	void construct(VertexArray& va) const override;
	void destruct(VertexArray& va) const override;

	void setAttribute(VertexArray& va, GLuint attribute, VertexComponentCount components, VertexComponentType type, GLuint relativeOffset, bool normalize = true) const override;
	void unsetAttribute(VertexArray& va, GLuint attribute) const override;

	void setBindingPoint(VertexArray& va, GLuint bindingPoint, const Buffer& buffer, GLintptr offset, GLsizei stride) const override;
	void unsetBindingPoint(VertexArray& va, GLuint bindingPoint) const override;

	void connectAttributeToBindingPoint(VertexArray& va, GLuint attribute, GLuint bindingPoint) const override;

	void bind(const VertexArray& va) const override;
	void unbind(const VertexArray& va) const override;

	void renderArrays(const VertexArray& va, Primitive prim, GLint first, GLsizei count) const override;
	void renderArrays(const VertexArray& va, Primitive prim, GLint first, GLsizei count, GLsizei instancecount) const override;

	void renderElements(const VertexArray& va, Primitive prim, GLsizei count, IndexType type, const void* indices) const override;
	void renderElements(const VertexArray& va, Primitive prim, GLsizei count, IndexType type, const void* indices, GLsizei instancecount) const override;

	void renderElementsBaseVertex(const VertexArray& va, Primitive prim, GLsizei count, IndexType type, const void* indices, GLint basevertex) const override;
	void renderElementsBaseVertex(const VertexArray& va, Primitive prim, GLsizei count, IndexType type, const void* indices, GLsizei instancecount, GLint basevertex) const override;

private:

	void throwIfNotCurrentlyBound(const VertexArray& va) const;

#ifdef _DEBUG
	static inline GLuint s_currentlyBoundID = 0u;
#endif // _DEBUG
};

}