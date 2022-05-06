/*! 
 *  \author    Dubsky Tomas
 */
#include <RealEngine/main/Error.hpp>

#include <iostream>
#include <string>

#include <SDL2/SDL.h>

#include <RealEngine/main/RealEngine.hpp>

namespace RE {

[[noreturn]] void fatalError(std::string_view error, int exitCode/* = EXIT_FAILURE*/) {
	std::cerr << error << std::endl;
	exit(EXIT_FAILURE);
}

void error(std::string_view error, bool appendEOL/* = true*/) {
	std::cerr << error;
	if (appendEOL) std::cerr << std::endl;
}

void log(std::string_view message, bool appendEOL/* = true*/) {
	std::cout << message;
	if (appendEOL) std::cout << '\n';
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

void GLAPIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
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