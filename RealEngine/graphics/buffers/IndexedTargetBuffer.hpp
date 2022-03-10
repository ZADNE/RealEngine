#pragma once

#include <RealEngine/graphics/buffers/Buffer.hpp>

namespace RE {

/**
 * @brief Is a buffer that binds to an indexed target instead of a generic target.
 *
 * Indexed target buffers always use immutable storage.
 *
 * @tparam BufferType	There are two types indexed target buffers: ATOMIC_COUNTER and TRANSFORM_FEEDBACK.
 *						For UNIFORM and SHADER_STORAGE use interface block buffer.
*/
template<BufferType type>
class IndexedTargetBuffer : public Buffer<type, RE::BufferStorage::IMMUTABLE> {
public:

	/**
	 * @brief Constructs an indexed target buffer of given size
	 * @param bindingPoint The indexed target that this buffer binds to
	 * @param bindNow If true, the constructed buffer is bound to its binding point upon creation
	 * @see Buffer::Buffer
	*/
	IndexedTargetBuffer(GLuint bindingPoint, bool bindNow, GLsizeiptr sizeInBytes, BufferUsageFlags flags, const void* data = nullptr) :
		Buffer<type>(sizeInBytes, flags, data),
		p_bindingPoint(bindingPoint) {
		static_assert(
			type == BufferType::ATOMIC_COUNTER ||
			type == BufferType::SHADER_STORAGE ||
			type == BufferType::TRANSFORM_FEEDBACK ||
			type == BufferType::UNIFORM
		);
		if (bindNow) bind();
	}

	/**
	 * @brief Contructs an indexed buffer as a storage for given type
	 * @param bindingPoint The indexed target that this buffer binds to
	 * @param bindNow If true, the constructed buffer is bound to its binding point upon creation
	 * @see Buffer::Buffer
	*/
	template<typename T>
	IndexedTargetBuffer(GLuint bindingPoint, bool bindNow, BufferUsageFlags flags, const T& data) :
		IndexedTargetBuffer(bindingPoint, bindNow, sizeof(T), flags, &data) {

	}

	/**
	 * @brief Constructs an indexed target buffer with given vector of data
	 * @param bindingPoint The indexed target that this buffer binds to
	 * @param bindNow If true, the constructed buffer is bound to its binding point upon creation
	 * @see Buffer::Buffer
	*/
	template<typename T>
	IndexedTargetBuffer(GLuint bindingPoint, bool bindNow, BufferUsageFlags flags, const std::vector<T>& data) :
		IndexedTargetBuffer(bindingPoint, bindNow, flags, data.size() * sizeof(T), data.data()) {
	}

	/**
	 * @brief Binds the buffer to its indexed binding point
	*/
	void bind() {
		glBindBufferBase(static_cast<GLenum>(type), p_bindingPoint, this->p_ID);
	}

protected:
	GLuint p_bindingPoint = 0u; /**< Indexed binding point (target) that this buffer binds to */
};

}