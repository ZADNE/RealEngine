/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <optional>

#include <glm/mat4x4.hpp>

#include <RealEngine/graphics/buffers/TypedBuffer.hpp>

namespace RE {

const BufferTypedIndex UNIF_BUF_VIEWPORT_MATRIX = {RE::BufferType::UNIFORM, 0u};/**< Binding point of the view matrix uniform buffer */

/**
 * @brief Controls transformation from Normalized device coordinates to Window coordinates
*/
class Viewport {
	friend class Window;
public:

	/**
	 * @brief Gets the uniform buffer that contains orthogonal view matrix of the window
	 *
	 * This can be used to connect custom shaders to window matrix.
	 *
	 * @return Uniform buffer with view matrix of this window
	*/
	static TypedBuffer& getWindowMatrixUniformBuffer() { return *s_windowMatrixUniformBuffer; }

	/**
	 * @brief Overwrites view matrix uniform buffer to match viewport
	*/
	static void setWindowMatrixToMatchViewport();

	/**
	 * @brief Sets viewport rectangle for further rendering
	 * @param pos Position of the rectangle
	 * @param size Size of the rectangle
	*/
	static void set(const glm::ivec2& pos, const glm::ivec2& size);

	/**
	 * @brief Sets viewport rectangle target whole window
	*/
	static void setToWholeWindow();
private:
	static std::optional<TypedBuffer> s_windowMatrixUniformBuffer;/**< Uniform buffer that contains view matrix of the window */
	static glm::ivec2 s_windowSize;
	static glm::mat4 s_windowMatrix;
};

}