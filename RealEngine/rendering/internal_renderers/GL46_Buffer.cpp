﻿/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/GL46_Buffer.hpp>

#include <cassert>

#include <GL/glew.h>

#include <RealEngine/rendering/buffers/Buffer.hpp>

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

namespace RE {

using enum BufferType;
using enum BufferStorage;
using enum BufferUsageFlags;
using enum BufferMapUsageFlags;

GLenum convert(BufferAccessFrequency accessFreq, BufferAccessNature accessNature) {
    static constexpr GLenum enums[] = {//I am not sure if these are quaranteed to be bit fields so I will do it ttihs way to be sure
        GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY,
        GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY,
        GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, GL_DYNAMIC_COPY
    };
    return enums[static_cast<int>(accessFreq) * 3 + static_cast<int>(accessNature)];
}

GLenum convert(BufferType bt) {
    switch (bt) {
    case ARRAY:                 return GL_ARRAY_BUFFER;
    case ATOMIC_COUNTER:        return GL_ATOMIC_COUNTER_BUFFER;
    case COPY_READ:             return GL_COPY_READ_BUFFER;
    case COPY_WRITE:            return GL_COPY_WRITE_BUFFER;
    case DISPATCH_INDIRECT:     return GL_DISPATCH_INDIRECT_BUFFER;
    case DRAW_INDIRECT:         return GL_DRAW_INDIRECT_BUFFER;
    case ELEMENT_ARRAY:         return GL_ELEMENT_ARRAY_BUFFER;
    case PIXEL_PACK:            return GL_PIXEL_PACK_BUFFER;
    case PIXEL_UNPACK:          return GL_PIXEL_UNPACK_BUFFER;
    case QUERY:                 return GL_QUERY_BUFFER;
    case SHADER_STORAGE:        return GL_SHADER_STORAGE_BUFFER;
    case TEXTURE:               return GL_TEXTURE_BUFFER;
    case TRANSFORM_FEEDBACK:    return GL_TRANSFORM_FEEDBACK_BUFFER;
    case UNIFORM:               return GL_UNIFORM_BUFFER;
    default: throw "Unknown buffer type!";
    }
}

GLbitfield convert(BufferUsageFlags flags) {
    GLbitfield flags_gl = 0;
    if (flags & DYNAMIC_STORAGE) flags_gl |= GL_DYNAMIC_STORAGE_BIT;
    if (flags & MAP_READ) flags_gl |= GL_MAP_READ_BIT;
    if (flags & MAP_WRITE) flags_gl |= GL_MAP_WRITE_BIT;
    if (flags & MAP_PERSISTENT) flags_gl |= GL_MAP_PERSISTENT_BIT;
    if (flags & MAP_COHERENT) flags_gl |= GL_MAP_COHERENT_BIT;
    if (flags & CLIENT_STORAGE) flags_gl |= GL_CLIENT_STORAGE_BIT;
    return flags_gl;
}

GLbitfield convert(BufferMapUsageFlags flags) {
    GLbitfield flags_gl = 0;
    if (flags & READ) flags_gl |= GL_MAP_READ_BIT;
    if (flags & WRITE) flags_gl |= GL_MAP_WRITE_BIT;
    if (flags & PERSISTENT) flags_gl |= GL_MAP_PERSISTENT_BIT;
    if (flags & COHERENT) flags_gl |= GL_MAP_COHERENT_BIT;
    if (flags & INVALIDATE_RANGE) flags_gl |= GL_MAP_INVALIDATE_RANGE_BIT;
    if (flags & INVALIDATE_BUFFER) flags_gl |= GL_MAP_INVALIDATE_BUFFER_BIT;
    if (flags & FLUSH_EXPLICIT) flags_gl |= GL_MAP_FLUSH_EXPLICIT_BIT;
    if (flags & UNSYNCHRONIZED) flags_gl |= GL_MAP_UNSYNCHRONIZED_BIT;
    return flags_gl;
}

BufferInternals GL46_Buffer::constructImmutable(size_t sizeInBytes, BufferUsageFlags flags, const void* data) const {
    unsigned int id;
    glCreateBuffers(1, &id);
    glNamedBufferStorage(id, sizeInBytes, data, convert(flags));
#ifdef _DEBUG
    return BufferInternals{id, sizeInBytes, 0, IMMUTABLE};
#else
    return BufferInternals{id, sizeInBytes, 0};
#endif // _DEBUG
}

BufferInternals GL46_Buffer::constructMutable(size_t sizeInBytes, BufferAccessFrequency accessFreq, BufferAccessNature accessNature, const void* data) const {
    unsigned int id;
    auto access = convert(accessFreq, accessNature);
    glCreateBuffers(1, &id);
    glNamedBufferData(id, sizeInBytes, data, access);
#ifdef _DEBUG
    return BufferInternals{id, sizeInBytes, access, MUTABLE};
#else
    return BufferInternals{id, sizeInBytes, access};
#endif // _DEBUG
}

void GL46_Buffer::destruct(BufferInternals& bf) const {
    glDeleteBuffers(1, &bf.m_id);
}

void GL46_Buffer::bind(const BufferInternals& bf, BufferType bindType) const {
    glBindBuffer(convert(bindType), bf.m_id);
}

void GL46_Buffer::bindIndexed(const BufferInternals& bf, const BufferTypedIndex& index) const {
#ifdef _DEBUG
    if (!isIndexedBufferType(index.type)) {
        throw "Indexed binding used on type that does not use it";
    }
#endif // _DEBUG
    glBindBufferBase(convert(index.type), index.bindingIndex, bf.m_id);
}

void GL46_Buffer::overwrite(const BufferInternals& bf, size_t offsetInBytes, size_t countBytes, const void* data) const {
    glInvalidateBufferSubData(bf.m_id, offsetInBytes, countBytes);
    glNamedBufferSubData(bf.m_id, offsetInBytes, countBytes, data);
}

void GL46_Buffer::redefine(BufferInternals& bf, size_t sizeInBytes, const void* data) const {
    assert(bf.m_storage == MUTABLE);
    if (sizeInBytes > bf.m_sizeInBytes) {
        bf.m_sizeInBytes = sizeInBytes;
        invalidate(bf);
        glNamedBufferData(bf.m_id, sizeInBytes, data, bf.m_access);
    } else {
        invalidate(bf, sizeInBytes);
        glNamedBufferSubData(bf.m_id, 0, sizeInBytes, data);
    }
}

void GL46_Buffer::invalidate(const BufferInternals& bf) const {
    glInvalidateBufferData(bf.m_id);
}

void GL46_Buffer::invalidate(const BufferInternals& bf, size_t lengthInBytes) const {
    glInvalidateBufferSubData(bf.m_id, 0, lengthInBytes);
}

void* GL46_Buffer::map(const BufferInternals& bf, size_t offsetInBytes, size_t lengthInBytes, BufferMapUsageFlags mappingUsage) const {
    return glMapNamedBufferRange(bf.m_id, offsetInBytes, lengthInBytes, convert(mappingUsage));
}

void GL46_Buffer::flushMapped(const BufferInternals& bf, size_t offsetInBytes, size_t lengthInBytes) const {
    glFlushMappedNamedBufferRange(bf.m_id, offsetInBytes, lengthInBytes);
}

bool GL46_Buffer::unmap(const BufferInternals& bf) const {
    auto rval = glUnmapNamedBuffer(bf.m_id);
#ifdef _DEBUG
    if (rval == GL_FALSE) {
        std::cerr << "Buffer unmapping failed!\n";
    }
#endif // _DEBUG
    return rval;
}

}