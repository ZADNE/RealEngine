#pragma once
#include <string>

#include <GL\glew.h>

namespace RE {

/**
 * Logs fatal error and ends program immediately.
 *
 * No-return function
 *
 * @param error Final error message
 * @param exitCode Program's exit code
 */
[[noreturn]] void fatalError(const std::string& error, int exitCode = EXIT_FAILURE);

/**
 * Logs an error
 *
 * @param error The error message
 * @param newLine If true, end of line will be appended at the end of message.
 */
void error(const std::string& error, bool appendEOL = true);

/**
 * Logs a message
 *
 * @param message The log message
 * @param newLine If true, end of line will be appended at the end of message.
 */
void log(const std::string& message, bool appendEOL = true);

//OpenGL-related

/**
 * Polls all graphics errors and logs them as errors.
 */
void checkForGraphicsErrors();

/**
 * OpenGL error callback function
 */
void GLAPIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

}