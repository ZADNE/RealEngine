#pragma once
#include <utility>
#include <vector>

#include <GL/glew.h>

namespace RE {

enum class BufferType : GLenum {
	ARRAY = GL_ARRAY_BUFFER,
	ATOMIC_COUNTER = GL_ATOMIC_COUNTER_BUFFER,
	COPY_READ = GL_COPY_READ_BUFFER,
	COPY_WRITE = GL_COPY_WRITE_BUFFER,
	DISPATCH_INDIRECT = GL_DISPATCH_INDIRECT_BUFFER,
	DRAW_INDIRECT = GL_DRAW_INDIRECT_BUFFER,
	ELEMENT_ARRAY = GL_ELEMENT_ARRAY_BUFFER,
	PIXEL_PACK = GL_PIXEL_PACK_BUFFER,
	PIXEL_UNPACK = GL_PIXEL_UNPACK_BUFFER,
	QUERY = GL_QUERY_BUFFER,
	SHADER_STORAGE = GL_SHADER_STORAGE_BUFFER,
	TEXTURE = GL_TEXTURE_BUFFER,
	TRANSFORM_FEEDBACK = GL_TRANSFORM_FEEDBACK_BUFFER,
	UNIFORM = GL_UNIFORM_BUFFER
};

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

GLenum bufferAccesToGLEnum(BufferAccessFrequency accessFreq, BufferAccessNature accessNature);

template<BufferType type, BufferStorage storage = BufferStorage::IMMUTABLE>
class Buffer {
	friend class VertexArray;
public:

	Buffer(GLsizeiptr sizeInBytes, BufferUsageFlags flags, const void* data = nullptr) :
		p_sizeInBytes(sizeInBytes) {
		static_assert(storage == IMMUTABLE);

		glCreateBuffers(1, &p_ID);

		glNamedBufferStorage(p_ID, p_sizeInBytes, data, static_cast<GLbitfield>(flags));
	}

	template<typename T>
	Buffer(BufferUsageFlags flags, const T& data) :
		Buffer(sizeof(T), flags, &data) {

	}

	template<typename T>
	Buffer(BufferUsageFlags flags, const std::vector<T>& data) :
		Buffer(data.size() * sizeof(T), flags, data.data()) {

	}

	Buffer(GLsizeiptr sizeInBytes, BufferAccessFrequency accessFreq, BufferAccessNature accessNature, const void* data = nullptr) :
		p_sizeInBytes(sizeInBytes),
		p_access(bufferAccesToGLEnum(accessFreq, accessNature)) {
		static_assert(storage == MUTABLE);

		glCreateBuffers(1, &p_ID);

		glNamedBufferData(p_ID, p_sizeInBytes, data, p_access);
	}

	Buffer(BufferAccessFrequency accessFreq, BufferAccessNature accessNature) :
		p_sizeInBytes(0),
		p_access(bufferAccesToGLEnum(accessFreq, accessNature)) {
		static_assert(storage == MUTABLE);

		glCreateBuffers(1, &p_ID);
	}

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

	void bind() {
	#ifdef _DEBUG
		if (p_currentlyBound.ID != 0) {
			throw "Overbound buffers";
		}
		p_currentlyBound.ID = p_ID;
	#endif // _DEBUG
		glBindBuffer(static_cast<GLenum>(type), p_ID);
	}

	void unbind() {
	#ifdef _DEBUG
		if (p_currentlyBound.ID != p_ID) {
			throw "Overbound buffers";
		}
		p_currentlyBound.ID = 0;
		glBindBuffer(static_cast<GLenum>(type), 0);
	#endif // _DEBUG
	}

	void overwrite(GLintptr offset, GLsizeiptr countBytes, const void* data) {
		glInvalidateBufferSubData(p_ID, offset, countBytes);
		glNamedBufferSubData(p_ID, offset, countBytes, data);
	}

	template<typename T>
	void overwrite(const T& data) {
		overwrite(0, sizeof(T), reinterpret_cast<const void*>(&data));
	}

	template<typename T>
	void overwrite(GLintptr offset, const std::vector<T>& data) {
		overwrite(offset, data.size() * sizeof(T), reinterpret_cast<const void*>(data.data()));
	}

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

	template<typename T>
	void redefine(const T& data) {
		redefine(sizeof(T), reinterpret_cast<const void*>(&data));
	}

	template<typename T>
	void redefine(const std::vector<T>& data) {
		redefine(data.size() * sizeof(T), reinterpret_cast<const void*>(data.data()));
	}

	void invalidate() {
		static_assert(storage == MUTABLE);
		glInvalidateBufferData(p_ID);
	}

	void invalidate(GLsizeiptr sizeInBytes) {
		static_assert(storage == MUTABLE);
		glInvalidateBufferSubData(p_ID, 0, sizeInBytes);
	}

protected:
	using enum BufferType;
	using enum BufferStorage;
	using enum BufferAccessFrequency;
	using enum BufferAccessNature;
	using enum BufferUsageFlags;

	GLuint p_ID = 0;

	GLsizeiptr p_sizeInBytes = 0;

	GLenum p_access = 0;

#ifdef _DEBUG
	template<BufferType> struct CurrentBinding {
		GLuint ID = 0;
	};
	static inline CurrentBinding<type> p_currentlyBound{};
#endif // _DEBUG
};

}