/*!
 *  @author    Dubsky Tomas
 */
#pragma once

#include <RealEngine/rendering/internal_renderers/GL46_Buffer.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_ShaderProgram.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_Texture.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_VertexArray.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_Viewport.hpp>

namespace RE {

/**
 * @brief Enforces use of OpenGL 4.6 graphics backend.
 * 
 * This is used internally when the RealEngine starts.
 *
 * @warning Never use this class directly!
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

	GL46_Buffer m_bufferImpl;
	GL46_ShaderProgram m_shaderProgramImpl;
	GL46_Texture m_textureImpl;
	GL46_VertexArray m_vertexArrayImpl;
	GL46_Viewport m_viewportImpl;
};

}