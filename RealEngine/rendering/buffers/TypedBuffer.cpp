﻿/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/buffers/TypedBuffer.hpp>

namespace RE {

void TypedBuffer::changeType(BufferType type) {
    m_index.type = type;
    m_index.bindingIndex = std::numeric_limits<unsigned int>::max();
}

void TypedBuffer::changeType(BufferTypedIndex index) {
    m_index = index;
}

void TypedBuffer::bind() {
    Buffer::bind(m_index.type);
}

void TypedBuffer::bindIndexed() {
    Buffer::bindIndexed(m_index);
}

}