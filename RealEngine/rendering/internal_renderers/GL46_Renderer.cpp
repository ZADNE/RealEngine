/*!
 *  @author    Dubsky Tomas
 */
#pragma once

#include <RealEngine/rendering/internal_renderers/GL46_Renderer.hpp>

#include <SDL2/SDL.h>

#include <RealEngine/rendering/buffers/Buffer.hpp>
#include <RealEngine/rendering/vertices/ShaderProgram.hpp>
#include <RealEngine/rendering/vertices/VertexArray.hpp>
#include <RealEngine/rendering/targets/Viewport.hpp>

namespace RE {

void GL46_Renderer::use() {
	if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)) {
		error("Cannot use doublebuffer!");
	}

#ifdef _DEBUG
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG)) {
		error("Cannot use debug context!");
	}
#endif // _DEBUG

	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE)) {
		error("Cannot use core profile context!");
	}
	static GL46_Renderer renderer;
}

GL46_Renderer::GL46_Renderer() {
	Buffer::s_impl = &m_buffer;
	ShaderProgram::s_impl = &m_shaderProgram;
	VertexArray::s_impl = &m_vertexArray;
	Viewport::s_impl = &m_viewport;
}

GL46_Renderer::~GL46_Renderer() {
	Buffer::s_impl = nullptr;
	ShaderProgram::s_impl = nullptr;
	VertexArray::s_impl = nullptr;
	Viewport::s_impl = nullptr;
}

}