/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <optional>

#include <RealEngine/rendering/internal_renderers/GL46_Buffer.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_Framebuffer.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_ShaderProgram.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_Texture.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_VertexArray.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_Viewport.hpp>

#include <RealEngine/rendering/output/Framebuffer.hpp>

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
	* @brief Tries to prepare for creation of OpenGL 4.6 context.
	* @return True if succeeded.
	* @warning Do not call use() if this fails.
	*
	* Requires SDL2 to be initialized.
	*
	* @note To be called only once at the start of the program.
	*/
	static bool prepare();

	/**
	* @brief Initializes the renderer.
	* @warning Do not call this if prepare() has failed.
	*/
	static void initialize();

private:
	GL46_Renderer();
	~GL46_Renderer();

	GL46_Renderer(const GL46_Renderer&) = delete;
	GL46_Renderer& operator=(const GL46_Renderer&) = delete;

	GL46_Buffer m_bufferImpl;
	GL46_Framebuffer m_mainFramebufferImpl;
	GL46_ShaderProgram m_shaderProgramImpl;
	GL46_Texture m_textureImpl;
	GL46_VertexArray m_vertexArrayImpl;
	GL46_Viewport m_viewportImpl;

	std::optional<Framebuffer> m_defaultFramebuffer;

	/**
	* OpenGL error callback function
	*/
	static void GLAPIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
};

/**
* @brief Polls all OpenGL errors and logs them as errors.
*/
void checkForGraphicsErrors();

}