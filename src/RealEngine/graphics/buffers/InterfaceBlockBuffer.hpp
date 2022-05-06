/*! 
 *  \author    Dubsky Tomas
 */
#pragma once

#include <RealEngine/graphics/buffers/IndexedTargetBuffer.hpp>
#include <RealEngine/graphics/ShaderProgram.hpp>

namespace RE {

/**
 * @brief Provides the backing storage for an interface block.
 *
 * Interface block buffers are a type of indexed target buffers.
 * @tparam BufferType There are two types interface block buffers: UNIFORM and SHADER_STORAGE.
*/
template<BufferType type>
class InterfaceBlockBuffer : public IndexedTargetBuffer<type> {
public:

	/**
	 * @brief Constructs an interface block buffer of given size
	 * @see IndexedTargetBuffer::IndexedTargetBuffer
	*/
	InterfaceBlockBuffer(GLuint bindingPoint, bool bindNow, GLsizeiptr sizeInBytes, BufferUsageFlags flags, const void* data = nullptr) :
		IndexedTargetBuffer<type>(bindingPoint, bindNow, sizeInBytes, flags, data) {
		static_assert(type == BufferType::UNIFORM || type == BufferType::SHADER_STORAGE);
	}

	/**
	 * @brief Contructs an interface block buffer as a storage for given type
	 * @see IndexedTargetBuffer::IndexedTargetBuffer
	*/
	template<typename T>
	InterfaceBlockBuffer(GLuint bindingPoint, bool bindNow, BufferUsageFlags flags, const T& data) :
		InterfaceBlockBuffer(bindingPoint, bindNow, sizeof(T), flags, &data) {

	}

	/**
	 * @brief Constructs an interface block buffer of given size
	 * @see IndexedTargetBuffer::IndexedTargetBuffer
	*/
	template<typename T>
	InterfaceBlockBuffer(GLuint bindingPoint, bool bindNow, BufferUsageFlags flags, const std::vector<T>& data) :
		InterfaceBlockBuffer(bindingPoint, bindNow, flags, data.size() * sizeof(T), data.data()) {
	}

	/**
	 * @brief Connects the buffer with an interface block within given shader program
	 * @param shaderProgram Program that contains the interface block
	 * @param interfaceBlockIndex Index of the interface block within the program
	*/
	void connectToShaderProgram(const ShaderProgram& shaderProgram, GLuint interfaceBlockIndex) const {
		if constexpr (type == BufferType::UNIFORM) {
			glUniformBlockBinding(shaderProgram.m_ID, interfaceBlockIndex, this->p_bindingPoint);
		} else if constexpr (type == BufferType::SHADER_STORAGE) {
			glShaderStorageBlockBinding(shaderProgram.m_ID, interfaceBlockIndex, this->p_bindingPoint);
		}
	}

};

}