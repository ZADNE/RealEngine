/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IVertexArray.hpp>

namespace RE {

class VertexArray {
	friend class GL46_Renderer;
	friend class GL46_VertexArray;
public:

	VertexArray();
	~VertexArray();

	VertexArray(const VertexArray&) = delete;
	VertexArray(VertexArray&& other) noexcept;

	VertexArray& operator=(const VertexArray&) = delete;
	VertexArray& operator=(VertexArray&& other) noexcept;


	void setAttribute(GLuint attribute, VertexComponentCount components, VertexComponentType type, GLuint relativeOffset, bool normalize = true);
	void unsetAttribute(GLuint attribute);

	void setBindingPoint(GLuint bindingPoint, const Buffer& buffer, GLintptr offset, GLsizei stride);
	void unsetBindingPoint(GLuint bindingPoint);

	void connectAttributeToBindingPoint(GLuint attribute, GLuint bindingPoint);

	void bind() const;
	void unbind() const;

	void renderArrays(Primitive prim, GLint first, GLsizei count) const;
	void renderArrays(Primitive prim, GLint first, GLsizei count, GLsizei instancecount) const;

	void renderElements(Primitive prim, GLsizei count, IndexType type, const void* offset) const;
	void renderElements(Primitive prim, GLsizei count, IndexType type, const void* offset, GLsizei instancecount) const;

private:
	GLuint m_ID = 0;

	static IVertexArray* s_impl;
};

}