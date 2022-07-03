/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/buffers/TypedBuffer.hpp>

namespace RE {

void TypedBuffer::changeType(BufferType type) {
	p_index.type = type;
	p_index.bindingIndex = std::numeric_limits<GLuint>::max();
}

void TypedBuffer::changeType(BufferTypedIndex index) {
	p_index = index;
}

void TypedBuffer::bind() {
	Buffer::bind(p_index.type);
}

void TypedBuffer::bindIndexed() {
	Buffer::bindIndexed(p_index);
}

}