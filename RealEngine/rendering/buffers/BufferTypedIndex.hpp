/*!
 *  @author    Dubsky Tomas
 */
#pragma once

namespace RE {

/**
 * @brief Lists all possible types of buffers
*/
enum class BufferType : unsigned int {
    ARRAY,
    ATOMIC_COUNTER,
    COPY_READ,
    COPY_WRITE,
    DISPATCH_INDIRECT,
    DRAW_INDIRECT,
    ELEMENT_ARRAY,
    PIXEL_PACK,
    PIXEL_UNPACK,
    QUERY,
    SHADER_STORAGE,
    TEXTURE,
    TRANSFORM_FEEDBACK,
    UNIFORM
};

/**
 * @brief Returns true if the given buffer type can be bound to an indexed binding point
*/
inline bool isIndexedBufferType(BufferType type) {
    switch (type) {
    case BufferType::ATOMIC_COUNTER:
    case BufferType::SHADER_STORAGE:
    case BufferType::TRANSFORM_FEEDBACK:
    case BufferType::UNIFORM:
        return true;
    default:
        return false;
    }
}

/**
 * @brief Is a POD struct with buffer type and binding index
*/
struct BufferTypedIndex {
    constexpr BufferTypedIndex(BufferType type, unsigned int bindingIndex) :
        type(type), bindingIndex(bindingIndex) {

    }

    BufferType type;
    unsigned int bindingIndex;
};

}