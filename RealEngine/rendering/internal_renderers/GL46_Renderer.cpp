/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/GL46_Renderer.hpp>

#include <iostream>

#include <SDL2/SDL.h>

#include <RealEngine/rendering/buffers/Buffer.hpp>
#include <RealEngine/rendering/output/MainFramebuffer.hpp>
#include <RealEngine/rendering/vertices/ShaderProgram.hpp>
#include <RealEngine/rendering/textures/Texture.hpp>
#include <RealEngine/rendering/vertices/VertexArray.hpp>
#include <RealEngine/rendering/output/Viewport.hpp>

namespace RE {

	bool GL46_Renderer::prepare() {
		if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)) {
			error("Could not use doublebuffer!"); return false;
		}

		int contextFlags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG | SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG;

#ifdef _DEBUG
		contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
#endif // _DEBUG

		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, contextFlags)) {
			error("Could not set context flags!"); return false;
		}

		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE)) {
			error("Could not use core profile context!"); return false;
		}

		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4)) {
			error("Could not use OpenGL 4!"); return false;
		}

		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6)) {
			error("Could not use OpenGL 4.6!"); return false;
		}

		return true;
	}

	void GL46_Renderer::initialize() {
		//Initialize GLEW
		if (glewInit() != GLEW_OK) {
			fatalError("GLEW failed initialization!");
		}

		//Print OpenGL info - should be OpenGL 4.6 (or higher? XD)
		std::printf("OpenGL:       %s\n", glGetString(GL_VERSION));
		std::printf("GLSL:         %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
		std::printf("Renderer:     %s\n", glGetString(GL_RENDERER));
		std::printf("Vendor:       %s\n", glGetString(GL_VENDOR));

#ifdef _DEBUG
		//Enable OpenGL error callbacks
		if (glDebugMessageCallback) {
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(openglCallbackFunction, nullptr);
			GLuint ids[] = {
				131185 //HORDCODE INGORE: Buffer object video memory notification
			};
			glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, sizeof(ids) / sizeof(GLuint), ids, GL_FALSE);
		}
#endif // _DEBUG

		//Disable dither that is probably no-op anyway
		glDisable(GL_DITHER);

		//Do not use multisample by default
		glDisable(GL_MULTISAMPLE);

		//Use blenbing by default
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Byte alignment
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		//Primitive restart index
		glEnable(GL_PRIMITIVE_RESTART);
		glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);

		static GL46_Renderer renderer;
	}

	GL46_Renderer::GL46_Renderer() {
		Buffer::s_impl = &m_bufferImpl;
		MainFramebuffer::s_impl = &m_mainFramebufferImpl;
		ShaderProgram::s_impl = &m_shaderProgramImpl;
		Texture::s_impl = &m_textureImpl;
		TextureProxy::s_impl = &m_textureImpl;
		VertexArray::s_impl = &m_vertexArrayImpl;
		Viewport::s_impl = &m_viewportImpl;
	}

	GL46_Renderer::~GL46_Renderer() {
		Buffer::s_impl = nullptr;
		MainFramebuffer::s_impl = nullptr;
		ShaderProgram::s_impl = nullptr;
		Texture::s_impl = nullptr;
		TextureProxy::s_impl = nullptr;
		VertexArray::s_impl = nullptr;
		Viewport::s_impl = nullptr;
	}

	void checkForGraphicsErrors() {
		GLenum err;
		do {
			err = glGetError();
			switch (err) {
			case GL_INVALID_ENUM:
				error("GL_INVALID_ENUM ENCOUNTERED!");
				break;
			case GL_INVALID_VALUE:
				error("GL_INVALID_VALUE ENCOUNTERED!");
				break;
			case GL_INVALID_OPERATION:
				error("GL_INVALID_OPERATION ENCOUNTERED!");
				break;
			case GL_STACK_OVERFLOW:
				error("GL_STACK_OVERFLOW ENCOUNTERED!");
				break;
			case GL_STACK_UNDERFLOW:
				error("GL_STACK_UNDERFLOW ENCOUNTERED!");
				break;
			case GL_OUT_OF_MEMORY:
				error("GL_OUT_OF_MEMORY ENCOUNTERED!");
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				error("GL_INVALID_FRAMEBUFFER_OPERATION ENCOUNTERED!");
				break;
			case GL_CONTEXT_LOST:
				error("GL_CONTEXT_LOST ENCOUNTERED!");
				break;
			case GL_TABLE_TOO_LARGE:
				error("GL_TABLE_TOO_LARGE ENCOUNTERED!");
				break;
			}
		} while (err != GL_NO_ERROR);
	}

	void GLAPIENTRY GL46_Renderer::openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
		std::cerr << "---OpenGL Callback Start---\n";
		std::cerr << "Message: " << message << '\n';
		std::cerr << "Type: ";
		switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			std::cerr << "ERROR";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			std::cerr << "DEPRECATED BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			std::cerr << "UNDEFINED BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			std::cerr << "PORTABILITY";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			std::cerr << "PERFORMANCE";
			break;
		case GL_DEBUG_TYPE_MARKER:
			std::cerr << "MARKER";
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			std::cerr << "PUSH GROUP";
			break;
		case  GL_DEBUG_TYPE_POP_GROUP:
			std::cerr << "POP GROUP";
			break;
		case GL_DEBUG_TYPE_OTHER:
			std::cerr << "OTHER";
			break;
		default:
			std::cerr << "UNKNOWN";
		}
		std::cerr << '\n';

		std::cerr << "Id: " << id << '\n';
		std::cerr << "Severity: ";
		switch (severity) {
		case GL_DEBUG_SEVERITY_LOW:
			std::cerr << "LOW";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			std::cerr << "MEDIUM";
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			std::cerr << "HIGH";
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			std::cerr << "NOTIFICATION";
			break;
		default:
			std::cerr << "UNKNOWN";
		}

		std::cerr << "\n----OpenGL Callback End----" << std::endl;
	}

}