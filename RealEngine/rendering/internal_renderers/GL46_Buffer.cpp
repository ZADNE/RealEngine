/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/GL46_Buffer.hpp>

#include <RealEngine/rendering/buffers/Buffer.hpp>

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

namespace RE {

using enum BufferType;
using enum BufferStorage;
using enum BufferAccessFrequency;
using enum BufferAccessNature;
using enum BufferUsageFlags;

int bufferTypeToIndex(BufferType type) {
	switch (type) {
	case ARRAY:					return 0;
	case ATOMIC_COUNTER:		return 1;
	case COPY_READ:				return 2;
	case COPY_WRITE:			return 3;
	case DISPATCH_INDIRECT:		return 4;
	case DRAW_INDIRECT:			return 5;
	case ELEMENT_ARRAY:			return 6;
	case PIXEL_PACK:			return 7;
	case PIXEL_UNPACK:			return 8;
	case QUERY:					return 9;
	case SHADER_STORAGE:		return 10;
	case TEXTURE:				return 11;
	case TRANSFORM_FEEDBACK:	return 12;
	case UNIFORM:				return 13;
	default:
		throw "Unknown buffer type!";
	}
}

void GL46_Buffer::constructImmutable(Buffer& bf, GLsizeiptr sizeInBytes, BufferUsageFlags flags, const void* data) const {
	bf.m_sizeInBytes = sizeInBytes;
	glCreateBuffers(1, &bf.m_ID);
	glNamedBufferStorage(bf.m_ID, bf.m_sizeInBytes, data, static_cast<GLbitfield>(flags));
#ifdef _DEBUG
	bf.m_storage = IMMUTABLE;
#endif // _DEBUG
}

void GL46_Buffer::constructMutable(Buffer& bf, GLsizeiptr sizeInBytes, BufferAccessFrequency accessFreq, BufferAccessNature accessNature, const void* data) const {
	bf.m_sizeInBytes = sizeInBytes;
	bf.m_access = bufferAccesToGLEnum(accessFreq, accessNature);
	glCreateBuffers(1, &bf.m_ID);
	glNamedBufferData(bf.m_ID, bf.m_sizeInBytes, data, bf.m_access);
#ifdef _DEBUG
	bf.m_storage = MUTABLE;
#endif // _DEBUG
}

void GL46_Buffer::destruct(Buffer& bf) const {
	glDeleteBuffers(1, &bf.m_ID);
}

void GL46_Buffer::bind(Buffer& bf, BufferType bindType) const {
	glBindBuffer(static_cast<GLenum>(bindType), bf.m_ID);
}

void GL46_Buffer::bindIndexed(Buffer& bf, const BufferTypedIndex& index) const {
#ifdef _DEBUG
	if (!isIndexedBufferType(index.type)) {
		throw "Indexed binding used on type that does not use it";
	}
#endif // _DEBUG
	glBindBufferBase(static_cast<GLenum>(index.type), index.bindingIndex, bf.m_ID);
}

void GL46_Buffer::overwrite(Buffer& bf, GLintptr offsetInBytes, GLsizeiptr countBytes, const void* data) const {
	glInvalidateBufferSubData(bf.m_ID, offsetInBytes, countBytes);
	glNamedBufferSubData(bf.m_ID, offsetInBytes, countBytes, data);
}

void GL46_Buffer::redefine(Buffer& bf, GLsizeiptr sizeInBytes, const void* data) const {
	assert(bf.m_storage == MUTABLE);
	if (sizeInBytes > bf.m_sizeInBytes) {
		bf.m_sizeInBytes = sizeInBytes;
		invalidate(bf);
		glNamedBufferData(bf.m_ID, sizeInBytes, data, bf.m_access);
	} else {
		invalidate(bf, sizeInBytes);
		glNamedBufferSubData(bf.m_ID, 0, sizeInBytes, data);
	}
}

void GL46_Buffer::invalidate(Buffer& bf) const {
	glInvalidateBufferData(bf.m_ID);
}

void GL46_Buffer::invalidate(Buffer& bf, GLsizeiptr lengthInBytes) const {
	glInvalidateBufferSubData(bf.m_ID, 0, lengthInBytes);
}

void* GL46_Buffer::map(Buffer& bf, GLintptr offsetInBytes, GLsizeiptr lengthInBytes, BufferMapUsageFlags mappingUsage) const {
	return glMapNamedBufferRange(bf.m_ID, offsetInBytes, lengthInBytes, static_cast<GLenum>(mappingUsage));
}

void GL46_Buffer::flushMapped(Buffer& bf, GLintptr offsetInBytes, GLsizeiptr lengthInBytes) const {
	glFlushMappedNamedBufferRange(bf.m_ID, offsetInBytes, lengthInBytes);
}

bool GL46_Buffer::unmap(Buffer& bf) const {
	auto rval = glUnmapNamedBuffer(bf.m_ID);
#ifdef _DEBUG
	if (rval == GL_FALSE) {
		std::cout << "Buffer unmapping failed!\n";
	}
#endif // _DEBUG
	return rval;
}

}