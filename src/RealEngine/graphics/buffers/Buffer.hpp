#pragma once
#include <utility>
#include <vector>

#include <RealEngine/graphics/buffers/BufferType.hpp>

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

/**
 * @brief Is a continuous block of memory stored in the GPU's memory.
 *
 * @tparam storage	A buffer can be either mutable or immutable. Mutable buffer can be resized,
 *					while immutable buffer cannot change its size upon construction.
*/
template<BufferType type, BufferStorage storage = BufferStorage::IMMUTABLE>
class Buffer {
	friend class VertexArray;
public:

	/**
	 * @brief Constructs an immutable buffer of given size, usage and data
	 * @param sizeInBytes Size in bytes of the buffer
	 * @param flags Restrict usage of the buffer
	 * @param data	If a valid pointer is provided, it is used to initialize the contents of the buffer.
	 *				If the nullptr is provided, the contents of the buffer are undefined.
	*/
	Buffer(GLsizeiptr sizeInBytes, BufferUsageFlags flags, const void* data = nullptr) :
		p_sizeInBytes(sizeInBytes) {
		static_assert(storage == IMMUTABLE);

		glCreateBuffers(1, &p_ID);

		glNamedBufferStorage(p_ID, p_sizeInBytes, data, static_cast<GLbitfield>(flags));
	}

	/**
	 * @brief Contructs an immutable buffer as a storage for given type
	 * @tparam T The type that the buffer will store (used to determine buffer's size)
	 * @param flags Restrict usage of the buffer
	 * @param data The instance of the type that the buffer will be initialized with
	*/
	template<typename T>
	Buffer(BufferUsageFlags flags, const T& data) :
		Buffer(sizeof(T), flags, &data) {

	}

	/**
	 * @brief Contructs an immutable buffer as an array
	 * @tparam T Element type of the array
	 * @param flags Restrict usage of the buffer
	 * @param data Array (provided as a vector) that determines the size and initial contents of the buffer
	*/
	template<typename T>
	Buffer(BufferUsageFlags flags, const std::vector<T>& data) :
		Buffer(data.size() * sizeof(T), flags, data.data()) {

	}

	/**
	 * @brief Constructs a mutable buffer of given initial size, access hints and initial data
	 * @param sizeInBytes Initial size of the buffer
	 * @param accessFreq Hints how often the buffer will be access by the GPU
	 * @param accessNature Hints how the buffer will be used
	 * @param data	If a valid pointer is provided, it is used to initialize the contents of the buffer.
	 *				If the nullptr is provided, the contents of the buffer are undefined.
	*/
	Buffer(GLsizeiptr sizeInBytes, BufferAccessFrequency accessFreq, BufferAccessNature accessNature, const void* data = nullptr) :
		p_sizeInBytes(sizeInBytes),
		p_access(bufferAccesToGLEnum(accessFreq, accessNature)) {
		static_assert(storage == MUTABLE);

		glCreateBuffers(1, &p_ID);

		glNamedBufferData(p_ID, p_sizeInBytes, data, p_access);
	}

	/**
	 * @brief Contructs a mutable buffer of given access hints
	 * @param accessFreq Hints how often the buffer will be access by the GPU
	 * @param accessNature Hints how the buffer will be used
	*/
	Buffer(BufferAccessFrequency accessFreq, BufferAccessNature accessNature) :
		p_sizeInBytes(0),
		p_access(bufferAccesToGLEnum(accessFreq, accessNature)) {
		static_assert(storage == MUTABLE);

		glCreateBuffers(1, &p_ID);
	}

	/**
	 * @brief Frees the backing memory block on the GPU and destructs the buffer
	*/
	~Buffer() {
		glDeleteBuffers(1, &p_ID);
	}

	Buffer(const Buffer&) = delete;

	Buffer(Buffer&& other) noexcept :
		p_ID(other.p_ID) {
		other.p_ID = 0;
	}

	Buffer& operator=(const Buffer&) = delete;

	Buffer& operator=(Buffer&& other) noexcept {
		std::swap(p_ID, other.p_ID);
		return *this;
	}

	/**
	 * @brief Binds the buffer to its generic binding point.
	 *
	 * Each type of buffer has its own generic binding point.
	*/
	template<BufferType bindType = type>
	void bind() {
	#ifdef _DEBUG
		if (p_currentlyBound.ID != 0) {
			throw "Overbound buffers";
		}
		p_currentlyBound.ID = p_ID;
	#endif // _DEBUG
		glBindBuffer(static_cast<GLenum>(bindType), p_ID);
	}

	/**
	 * @brief Unbinds the buffer from its binding point.
	 *
	 * A bound buffer has to be unbound before another buffer can be bound.
	*/
	template<BufferType bindType = type>
	void unbind() {
	#ifdef _DEBUG
		if (p_currentlyBound.ID != p_ID) {
			throw "Overbound buffers";
		}
		p_currentlyBound.ID = 0;
		glBindBuffer(static_cast<GLenum>(bindType), 0);
	#endif // _DEBUG
	}

	/**
	 * @brief Overwrites a portion of or whole buffer
	 * @param offsetInBytes Offset in bytes within the buffer to overwrite
	 * @param countBytes Number of bytes to overwrite
	 * @param data The data to overwrite with
	*/
	void overwrite(GLintptr offsetInBytes, GLsizeiptr countBytes, const void* data) {
		glInvalidateBufferSubData(p_ID, offsetInBytes, countBytes);
		glNamedBufferSubData(p_ID, offsetInBytes, countBytes, data);
	}

	/**
	 * @brief Overwrites the buffer with instance of a type
	 * @tparam T The type to overwrite the buffer with
	 * @param offsetInBytes Offset in bytes within the buffer to overwrite
	 * @param data The instance to overwrite the buffer with
	*/
	template<typename T>
	void overwrite(GLintptr offsetInBytes, const T& data) {
		overwrite(offsetInBytes, sizeof(T), reinterpret_cast<const void*>(&data));
	}

	/**
	 * @brief Overwrites the buffer with an array
	 * @tparam T Element type of the array
	 * @param offsetInBytes Offset in bytes within the buffer to overwrite
	 * @param data Array (provided as a vector) to overwrite the buffer with
	*/
	template<typename T>
	void overwrite(GLintptr offsetInBytes, const std::vector<T>& data) {
		overwrite(offsetInBytes, data.size() * sizeof(T), reinterpret_cast<const void*>(data.data()));
	}

	/**
	 * @brief Ensures that the buffer is big enough to store the data and writes it
	 *
	 * Since this operation may cause resize of the buffer,
	 * it can only be used for mutable buffers.
	 * @param sizeInBytes Size of the data required to write
	 * @param data The data to write
	*/
	void redefine(GLsizeiptr sizeInBytes, const void* data) {
		static_assert(storage == MUTABLE);
		if (sizeInBytes > p_sizeInBytes) {
			p_sizeInBytes = sizeInBytes;
			invalidate();
			glNamedBufferData(p_ID, sizeInBytes, data, p_access);
		} else {
			invalidate(sizeInBytes);
			glNamedBufferSubData(p_ID, 0, sizeInBytes, data);
		}
	}

	/**
	 * @brief Ensures that the buffer is big enough to store the data and writes it
	 * @tparam T Type to write
	 * @param data Instance to write
	*/
	template<typename T>
	void redefine(const T& data) {
		redefine(sizeof(T), reinterpret_cast<const void*>(&data));
	}

	/**
	 * @brief Ensures that the buffer is big enough to store the data and writes it
	 * @tparam T Element type of the array
	 * @param data Array (provided as a vector) to redefine the buffer as
	*/
	template<typename T>
	void redefine(const std::vector<T>& data) {
		redefine(data.size() * sizeof(T), reinterpret_cast<const void*>(data.data()));
	}

	/**
	 * @brief Makes the contents of the buffer undefined
	 *
	 * This effectively frees the backing block of memory of the buffer.
	*/
	void invalidate() {
		static_assert(storage == MUTABLE);
		glInvalidateBufferData(p_ID);
	}

	/**
	 * @brief Makes a range of the buffer undefined
	 * @param lengthInBytes Length of the range at the beginning of the buffer to invalidate
	*/
	void invalidate(GLsizeiptr lengthInBytes) {
		static_assert(storage == MUTABLE);
		glInvalidateBufferSubData(p_ID, 0, lengthInBytes);
	}

	/**
	 * @brief Maps a range of the buffer to the client's memory
	 * @tparam T Reinterpreted type of the returned pointer
	*/
	template<typename T>
	T* map(GLintptr offsetInBytes, GLsizeiptr lengthInBytes, BufferMapUsageFlags mappingUsage) {
		return reinterpret_cast<T*>(glMapNamedBufferRange(p_ID, offsetInBytes, lengthInBytes, static_cast<GLenum>(mappingUsage)));
	}

	/**
	 * @brief Indicates modifications to a mapped range of the buffer
	*/
	void flushMapped(GLintptr offsetInBytes, GLsizeiptr lengthInBytes) {
		glFlushMappedNamedBufferRange(p_ID, offsetInBytes, lengthInBytes);
	}

	/**
	 * @brief Releases the mapping of the buffer
	 * @return True if success. Buffer's contents are undefined if false is returned.
	*/
	bool unmap() {
		return glUnmapNamedBuffer(p_ID);
	}

protected:
	using enum BufferType;
	using enum BufferStorage;
	using enum BufferAccessFrequency;
	using enum BufferAccessNature;
	using enum BufferUsageFlags;

	GLuint p_ID = 0;				/**< Internal name of the buffer */
	GLsizeiptr p_sizeInBytes = 0;	/**< Size of the buffer */
	GLenum p_access = 0;			/**< Access hints of the buffer; relevant only for mutable buffers */

#ifdef _DEBUG
	template<BufferType> struct CurrentBinding {
		GLuint ID = 0;
	};
	static inline CurrentBinding<type> p_currentlyBound{};
#endif // _DEBUG
};

}