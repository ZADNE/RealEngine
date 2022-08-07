﻿/*!
 *  @author    Dubsky Tomas
 */
#pragma once

#include <RealEngine/graphics/renderers/GL46_Renderer.hpp>

#include <SDL2/SDL.h>

#include <RealEngine/graphics/ShaderProgram.hpp>
#include <RealEngine/graphics/VertexArray.hpp>

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
	ShaderProgram::s_impl = &m_shaderProgram;
	VertexArray::s_impl = &m_vertexArray;
}

GL46_Renderer::~GL46_Renderer() {
	ShaderProgram::s_impl = nullptr;
	VertexArray::s_impl = nullptr;
}

}