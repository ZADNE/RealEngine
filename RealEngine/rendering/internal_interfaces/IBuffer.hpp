/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <utility>
#include <vector>

#include <RealEngine/rendering/buffers/types.hpp>

namespace RE {

enum class BufferStorage {
	IMMUTABLE,
	MUTABLE
};

enum class BufferAccessFrequency {
	STREAM = 0,
	STATIC = 1,
	DYNAMIC = 2
};

enum class BufferAccessNature {
	DRAW = 0,
	READ = 1,
	COPY = 2
};

enum class BufferUsageFlags : GLbitfield {
	NO_FLAGS = 0u,
	DYNAMIC_STORAGE = GL_DYNAMIC_STORAGE_BIT,
	MAP_READ = GL_MAP_READ_BIT,
	MAP_WRITE = GL_MAP_WRITE_BIT,
	MAP_PERSISTENT = GL_MAP_PERSISTENT_BIT,
	MAP_COHERENT = GL_MAP_COHERENT_BIT,
	CLIENT_STORAGE = GL_CLIENT_STORAGE_BIT,
};

inline BufferUsageFlags operator|(BufferUsageFlags a, BufferUsageFlags b) {
	return static_cast<BufferUsageFlags>(static_cast<GLbitfield>(a) | static_cast<GLbitfield>(b));
}

/**
 * @brief Restricts the usage of a buffer mapping
*/
enum class BufferMapUsageFlags : GLbitfield {
	READ = GL_MAP_READ_BIT,								/**< Indicates that the mapping may be used to read buffer's data */
	WRITE = GL_MAP_WRITE_BIT,							/**< Indicates that the mapping may be used to modify buffer's data */
	PERSISTENT = GL_MAP_PERSISTENT_BIT,					/**< Indicates that the client intends to hold and use the mapping during subsequent GL operation */
	COHERENT = GL_MAP_COHERENT_BIT,						/**< Indicates that a persistent mapping is also to be coherent */
	INVALIDATE_RANGE = GL_MAP_INVALIDATE_RANGE_BIT,		/**< Indicates that the previous contents of the specified range may be discarded */
	INVALIDATE_BUFFER = GL_MAP_INVALIDATE_BUFFER_BIT,	/**< Indicates that the previous contents of the entire buffer may be discarded */
	FLUSH_EXPLICIT = GL_MAP_FLUSH_EXPLICIT_BIT,			/**< Indicates that one or more discrete subranges of the mapping may be modified */
	UNSYNCHRONIZED = GL_MAP_UNSYNCHRONIZED_BIT			/**< Indicates that the GL should not attempt to synchronize pending operations on the buffer */
};

inline BufferMapUsageFlags operator|(BufferMapUsageFlags a, BufferMapUsageFlags b) {
	return static_cast<BufferMapUsageFlags>(static_cast<GLbitfield>(a) | static_cast<GLbitfield>(b));
}

inline GLenum bufferAccesToGLEnum(BufferAccessFrequency accessFreq, BufferAccessNature accessNature) {
	static const GLenum enums[] = {//I am not sure if these are quaranteed to be bit fields so I will do it ttihs way to be sure
		GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY,
		GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY,
		GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, GL_DYNAMIC_COPY
	};
	return enums[static_cast<int>(accessFreq) * 3 + static_cast<int>(accessNature)];
}

class Buffer;

/**
* @brief Is a renderer-agnostic interface to Buffer implementation.
*
* This is used internally by Buffer.
*
* @see RE::Buffer
*/
class IBuffer {
public:

	virtual void constructImmutable(Buffer& bf, GLsizeiptr sizeInBytes, BufferUsageFlags flags, const void* data) const = 0;
	virtual void constructMutable(Buffer& bf, GLsizeiptr sizeInBytes, BufferAccessFrequency accessFreq, BufferAccessNature accessNature, const void* data) const = 0;
	virtual void destruct(Buffer& bf) const = 0;

	virtual void bind(Buffer& bf, BufferType bindType) const = 0;
	virtual void bindIndexed(Buffer& bf, const BufferTypedIndex& index) const = 0;

	virtual void overwrite(Buffer& bf, GLintptr offsetInBytes, GLsizeiptr countBytes, const void* data) const = 0;

	virtual void redefine(Buffer& bf, GLsizeiptr sizeInBytes, const void* data) const = 0;

	virtual void invalidate(Buffer& bf) const = 0;
	virtual void invalidate(Buffer& bf, GLsizeiptr lengthInBytes) const = 0;

	virtual void* map(Buffer& bf, GLintptr offsetInBytes, GLsizeiptr lengthInBytes, BufferMapUsageFlags mappingUsage) const = 0;
	virtual void flushMapped(Buffer& bf, GLintptr offsetInBytes, GLsizeiptr lengthInBytes) const = 0;
	virtual bool unmap(Buffer& bf) const = 0;
};

}