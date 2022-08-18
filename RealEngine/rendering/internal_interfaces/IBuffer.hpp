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

enum class BufferUsageFlags : unsigned int {
	NO_FLAGS = 0,
	DYNAMIC_STORAGE = 1,
	MAP_READ = 2,
	MAP_WRITE = 4,
	MAP_PERSISTENT = 8,
	MAP_COHERENT = 16,
	CLIENT_STORAGE = 32
};

inline BufferUsageFlags operator|(BufferUsageFlags a, BufferUsageFlags b) {
	return static_cast<BufferUsageFlags>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline bool operator&(BufferUsageFlags a, BufferUsageFlags b) {
	return static_cast<unsigned int>(a) & static_cast<unsigned int>(b);
}

/**
 * @brief Restricts the usage of a buffer mapping
*/
enum class BufferMapUsageFlags : unsigned int {
	READ = 1,					/**< Indicates that the mapping may be used to read buffer's data */
	WRITE = 2,					/**< Indicates that the mapping may be used to modify buffer's data */
	PERSISTENT = 4,				/**< Indicates that the client intends to hold and use the mapping during subsequent GL operation */
	COHERENT = 8,				/**< Indicates that a persistent mapping is also to be coherent */
	INVALIDATE_RANGE = 16,		/**< Indicates that the previous contents of the specified range may be discarded */
	INVALIDATE_BUFFER = 32,		/**< Indicates that the previous contents of the entire buffer may be discarded */
	FLUSH_EXPLICIT = 64,		/**< Indicates that one or more discrete subranges of the mapping may be modified */
	UNSYNCHRONIZED = 128		/**< Indicates that the GL should not attempt to synchronize pending operations on the buffer */
};

inline BufferMapUsageFlags operator|(BufferMapUsageFlags a, BufferMapUsageFlags b) {
	return static_cast<BufferMapUsageFlags>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline bool operator&(BufferMapUsageFlags a, BufferMapUsageFlags b) {
	return static_cast<unsigned int>(a) & static_cast<unsigned int>(b);
}

class Buffer;

/**
* @brief Is a renderer-agnostic interface to Buffer's implementation.
*
* This is used internally by Buffer.
*
* @see RE::Buffer
*/
class IBuffer {
public:

	virtual void constructImmutable(Buffer& bf, int sizeInBytes, BufferUsageFlags flags, const void* data) const = 0;
	virtual void constructMutable(Buffer& bf, int sizeInBytes, BufferAccessFrequency accessFreq, BufferAccessNature accessNature, const void* data) const = 0;
	virtual void destruct(Buffer& bf) const = 0;

	virtual void bind(Buffer& bf, BufferType bindType) const = 0;
	virtual void bindIndexed(Buffer& bf, const BufferTypedIndex& index) const = 0;

	virtual void overwrite(Buffer& bf, int offsetInBytes, int countBytes, const void* data) const = 0;

	virtual void redefine(Buffer& bf, int sizeInBytes, const void* data) const = 0;

	virtual void invalidate(Buffer& bf) const = 0;
	virtual void invalidate(Buffer& bf, int lengthInBytes) const = 0;

	virtual void* map(Buffer& bf, int offsetInBytes, int lengthInBytes, BufferMapUsageFlags mappingUsage) const = 0;
	virtual void flushMapped(Buffer& bf, int offsetInBytes, int lengthInBytes) const = 0;
	virtual bool unmap(Buffer& bf) const = 0;

};

}