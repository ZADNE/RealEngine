#include <RealEngine/graphics/UniformBuffer.hpp>

namespace RE {

UniformBuffer::UniformBuffer(GLuint bindingPoint, bool bindNow, BufferUsageFlags flags, GLsizeiptr sizeInBytes, const void* data/* = nullptr*/) :
	Buffer(sizeInBytes, flags, data),
	m_bindingPoint(bindingPoint) {
	if (bindNow) bind();
}

void UniformBuffer::connectToShaderProgram(const ShaderProgram& shaderProgram, GLuint interfaceBlockIndex) const {
	glUniformBlockBinding(shaderProgram.m_ID, interfaceBlockIndex, m_bindingPoint);
}

void UniformBuffer::bind() {
	glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingPoint, p_ID);
}

}