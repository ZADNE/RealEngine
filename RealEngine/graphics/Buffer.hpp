#pragma once
#include <utility>

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

	using enum BufferType;
	using enum BufferStorage;
	using enum BufferAccessFrequency;
	using enum BufferAccessNature;
	using enum BufferUsageFlags;

	Buffer(GLsizeiptr sizeInBytes, BufferUsageFlags flags, const void* data = nullptr) :
		m_sizeInBytes(sizeInBytes) {
		static_assert(storage == IMMUTABLE);

		glCreateBuffers(1, &m_ID);

		glNamedBufferStorage(m_ID, m_sizeInBytes, data, static_cast<GLbitfield>(flags));
	}

	Buffer(GLsizeiptr sizeInBytes, BufferAccessFrequency accessFreq, BufferAccessNature accessNature, const void* data = nullptr) :
		m_sizeInBytes(sizeInBytes),
		m_access(bufferAccesToGLEnum(accessFreq, accessNature)) {
		static_assert(storage == MUTABLE);

		glCreateBuffers(1, &m_ID);

		glNamedBufferData(m_ID, m_sizeInBytes, data, m_access);
	}

	~Buffer() {
		glDeleteBuffers(1, &m_ID);
	}

	Buffer(const Buffer&) = delete;

	Buffer(Buffer&& other) noexcept :
		m_ID(other.m_ID) {
		other.m_ID = 0;
	}

	Buffer& operator=(const Buffer&) = delete;

	Buffer& operator=(Buffer&& other) noexcept {
		std::swap(m_ID, other.m_ID);
		return *this;
	}

	void bind() {
	#ifdef _DEBUG
		if (m_currentlyBound.ID != 0) {
			throw "Overbound buffers";
		}
		m_currentlyBound.ID = m_ID;
	#endif // _DEBUG
		glBindBuffer(static_cast<GLenum>(type), m_ID);
	}

	void unbind() {
	#ifdef _DEBUG
		if (m_currentlyBound.ID != m_ID) {
			throw "Overbound buffers";
		}
		m_currentlyBound.ID = 0;
		glBindBuffer(static_cast<GLenum>(type), 0);
	#endif // _DEBUG
	}

	void overwrite(GLsizeiptr countBytes, const void* data) {
		glInvalidateBufferSubData(m_ID, 0, countBytes);
		glNamedBufferSubData(m_ID, 0, countBytes, data);
	}

	void redefine(GLsizeiptr sizeInBytes, const void* data) {
		static_assert(storage == MUTABLE);
		if (sizeInBytes > m_sizeInBytes) {
			m_sizeInBytes = sizeInBytes;
			glInvalidateBufferData(m_ID);
			glNamedBufferData(m_ID, sizeInBytes, data, m_access);
		} else {
			glInvalidateBufferSubData(m_ID, 0, sizeInBytes);
			glNamedBufferSubData(m_ID, 0, sizeInBytes, data);
		}
	}

	/*GLuint getID() {
		return m_ID;
	}*/

private:
	GLuint m_ID = 0;

	GLsizeiptr m_sizeInBytes = 0;

	GLenum m_access = 0;

#ifdef _DEBUG
	template<BufferType> struct CurrentBinding {
		GLuint ID = 0;
	};
	static inline CurrentBinding<type> m_currentlyBound{};
#endif // _DEBUG
};

}