/*!
 *  @author    Dubsky Tomas
 */
#pragma once

#include <RealEngine/graphics/renderers/GL46_ShaderProgram.hpp>

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

	GL46_ShaderProgram m_shaderProgram;
};

}