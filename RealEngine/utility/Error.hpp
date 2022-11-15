/*!
 *  @author    Dubsky Tomas
 *  @file
 */
#pragma once
#include <stdlib.h>

#include <stdexcept>
#include <string_view>

namespace RE {

/**
 * Logs fatal error and ends program immediately.
 *
 * No-return function
 *
 * @param error Final error message
 * @param exitCode Program's exit code
 */
[[noreturn]] void fatalError(std::string_view error, int exitCode = EXIT_FAILURE);

/**
 * Logs an error
 *
 * @param error The error message
 * @param newLine If true, end of line will be appended at the end of message.
 */
void error(std::string_view error, bool appendEOL = true);

/**
 * Logs a message
 *
 * @param message The log message
 * @param newLine If true, end of line will be appended at the end of message.
 */
void log(std::string_view message, bool appendEOL = true);

/**
 * @brief Is the base class for exceptions thrown by RealEngine
*/
class Exception : public std::runtime_error {
public:

    Exception(const char* str) :
        std::runtime_error(str) {
    }

    Exception(const std::string& str) :
        std::runtime_error(str) {
    }
};


}
