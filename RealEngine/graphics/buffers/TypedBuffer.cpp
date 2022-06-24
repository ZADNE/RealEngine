/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/buffers/TypedBuffer.hpp>

namespace RE {

void TypedBuffer::changeType(BufferType type) {
	p_type = type;
	p_bindingIndex = std::numeric_limits<GLuint>::max();
}

void TypedBuffer::changeType(BufferTypedIndex typedIndex) {
	p_type = typedIndex.type;
	p_bindingIndex = typedIndex.bindingIndex;
}

void TypedBuffer::bind() {
	Buffer::bind(p_type);
}

void TypedBuffer::unbind() {
	Buffer::unbind(p_type);
}

void TypedBuffer::bindIndexed() {
	Buffer::bindIndexed(p_type, p_bindingIndex);
}

void TypedBuffer::connectToInterfaceBlock(const ShaderProgram& shaderProgram, GLuint interfaceBlockIndex) {
	Buffer::connectToInterfaceBlock(shaderProgram, interfaceBlockIndex, p_type, p_bindingIndex);
}

}