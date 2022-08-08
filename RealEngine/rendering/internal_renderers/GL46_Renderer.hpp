/*!
 *  @author    Dubsky Tomas
 */
#pragma once

#include <RealEngine/rendering/internal_renderers/GL46_Buffer.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_ShaderProgram.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_VertexArray.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_Viewport.hpp>

namespace RE {

/**
 * @brief Enforces use of OpenGL 4.6 graphics backend.
 *
 * @warning Do not use this class directly!
*/
class GL46_Renderer {
public:
	/**
	 * @brief To be called only once at the start of the program.
	*/
	static void use();

private:
	GL46_Renderer();
	~GL46_Renderer();

	GL46_Renderer(const GL46_Renderer&) = delete;
	GL46_Renderer& operator=(const GL46_Renderer&) = delete;

	GL46_Buffer m_buffer;
	GL46_ShaderProgram m_shaderProgram;
	GL46_VertexArray m_vertexArray;
	GL46_Viewport m_viewport;
};

}