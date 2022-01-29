#pragma once

#include <RealEngine/graphics/ShaderProgram.hpp>
#include <RealEngine/graphics/Buffer.hpp>

namespace RE {

/**
 * @brief Allows sharing of uniforms between multiple programs.
 *
 * Uniform buffers always must have immutable storage.
*/
class UniformBuffer : public Buffer<RE::BufferType::UNIFORM, RE::BufferStorage::IMMUTABLE> {
	friend class ShaderProgram;
public:

	/**
	 * @brief Constructs uniform buffer
	 * @param bindingPoint Binding point that this uniform buffer will be bound to when bind() is used
	 * @param bindNow The constructed buffer is bound to its binding point if true.
	 * @see Buffer
	*/
	UniformBuffer(GLuint bindingPoint, bool bindNow, BufferUsageFlags flags, GLsizeiptr sizeInBytes, const void* data = nullptr);

	/**
	 * @brief Constructs uniform buffer
	 * @param bindingPoint Binding point that this uniform buffer will be bound to when bind() is used
	 * @param bindNow The constructed buffer is bound to its binding point if true.
	 * @see Buffer
	*/
	template<typename T>
	UniformBuffer(GLuint bindingPoint, bool bindNow, BufferUsageFlags flags, const std::vector<T>& data) :
		UniformBuffer(bindingPoint, bindNow, flags, data.size() * sizeof(T), data.data()) {

	}

	/**
	 * @brief Connects this uniform buffer to an interface block within given shader program
	 * @param shaderProgram Shader program that contains the interface block
	 * @param uniformBlockIndex Index of the uniform block within the program to connect this to
	*/
	void connectToShaderProgram(const ShaderProgram& shaderProgram, GLuint uniformBlockIndex) const;

	/**
	 * @brief Binds the buffer to its binding point
	*/
	void bind();

private:
	GLuint m_bindingPoint = 0u; /**< Indexed binding point that this buffer binds to */
};

}