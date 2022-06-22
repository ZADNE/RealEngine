/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/buffers/Buffer.hpp>


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

Buffer::Buffer(GLsizeiptr sizeInBytes, BufferUsageFlags flags, const void* data/* = nullptr*/) :
	p_sizeInBytes(sizeInBytes) {
	glCreateBuffers(1, &p_ID);
	glNamedBufferStorage(p_ID, p_sizeInBytes, data, static_cast<GLbitfield>(flags));
#ifdef _DEBUG
	p_storage = IMMUTABLE;
#endif // _DEBUG
}


Buffer::Buffer(GLsizeiptr sizeInBytes, BufferAccessFrequency accessFreq, BufferAccessNature accessNature, const void* data/* = nullptr*/) :
	p_sizeInBytes(sizeInBytes),
	p_access(bufferAccesToGLEnum(accessFreq, accessNature)) {
	glCreateBuffers(1, &p_ID);
	glNamedBufferData(p_ID, p_sizeInBytes, data, p_access);
#ifdef _DEBUG
	p_storage = MUTABLE;
#endif // _DEBUG
}

Buffer::~Buffer() {
	glDeleteBuffers(1, &p_ID);
}

Buffer::Buffer(Buffer&& other) noexcept :
	p_ID(other.p_ID) {
	other.p_ID = 0;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept {
	std::swap(p_ID, other.p_ID);
	return *this;
}

void Buffer::bind(BufferType bindType) {
#ifdef _DEBUG
	if (p_currentlyBound[bufferTypeToIndex(bindType)] != 0) {
		throw "Overbound buffers";
	}
	p_currentlyBound[bufferTypeToIndex(bindType)] = p_ID;
#endif // _DEBUG
	glBindBuffer(static_cast<GLenum>(bindType), p_ID);
}

void Buffer::unbind(BufferType bindType) {
#ifdef _DEBUG
	if (p_currentlyBound[bufferTypeToIndex(bindType)] != p_ID) {
		throw "Overbound buffers";
	}
	p_currentlyBound[bufferTypeToIndex(bindType)] = 0;
	glBindBuffer(static_cast<GLenum>(bindType), 0);
#endif // _DEBUG
}

void Buffer::bindIndexed(BufferType bindType, GLuint index) {
#ifdef _DEBUG
	switch (bindType) {
	case ATOMIC_COUNTER:
	case TRANSFORM_FEEDBACK:
	case UNIFORM:
	case SHADER_STORAGE:
		break;
	default:
		throw "Indexed binding used on type that does not use it";
	}
#endif // _DEBUG
	glBindBufferBase(static_cast<GLenum>(bindType), index, p_ID);
}

void Buffer::connectToInterfaceBlock(const ShaderProgram& shaderProgram, GLuint interfaceBlockIndex, BufferType bufferType, GLuint bindingIndex) const {
	if (bufferType == UNIFORM) {
		glUniformBlockBinding(shaderProgram.m_ID, interfaceBlockIndex, bindingIndex);
	} else if (bufferType == SHADER_STORAGE) {
		glShaderStorageBlockBinding(shaderProgram.m_ID, interfaceBlockIndex, bindingIndex);
	}
}

void Buffer::overwrite(GLintptr offsetInBytes, GLsizeiptr countBytes, const void* data) {
	glInvalidateBufferSubData(p_ID, offsetInBytes, countBytes);
	glNamedBufferSubData(p_ID, offsetInBytes, countBytes, data);
}

void Buffer::redefine(GLsizeiptr sizeInBytes, const void* data) {
	assert(p_storage == MUTABLE);
	if (sizeInBytes > p_sizeInBytes) {
		p_sizeInBytes = sizeInBytes;
		invalidate();
		glNamedBufferData(p_ID, sizeInBytes, data, p_access);
	} else {
		invalidate(sizeInBytes);
		glNamedBufferSubData(p_ID, 0, sizeInBytes, data);
	}
}

void Buffer::invalidate() {
	glInvalidateBufferData(p_ID);
}

void Buffer::invalidate(GLsizeiptr lengthInBytes) {
	glInvalidateBufferSubData(p_ID, 0, lengthInBytes);
}

void Buffer::flushMapped(GLintptr offsetInBytes, GLsizeiptr lengthInBytes) {
	glFlushMappedNamedBufferRange(p_ID, offsetInBytes, lengthInBytes);
}

bool Buffer::unmap() {
	return glUnmapNamedBuffer(p_ID);
}

}