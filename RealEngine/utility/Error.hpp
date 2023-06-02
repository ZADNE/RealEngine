﻿/*!
 *  @author    Dubsky Tomas
 *  @file
 */
#pragma once
#include <stdlib.h>

#include <stdexcept>
#include <string_view>

namespace re {

/**
 * @brief Logs a fatal error and ends program immediately.
 * @param error Final error message
 * @param exitCode Program's exit code
 * @note This function never returns.
 */
[[noreturn]]
void fatalError(std::string_view error, int exitCode = EXIT_FAILURE);

/**
 * @brief Logs an error
 * @param error The error message
 * @param newLine If true, end of line will be appended at the end of message.
 */
void error(std::string_view error, bool appendEOL = true);

/**
 * @brief Logs a message
 * @param message The log message
 * @param newLine If true, end of line will be appended at the end of message.
 */
void log(std::string_view message, bool appendEOL = true);

/**
 * @brief Is the base class for exceptions thrown by RealEngine
*/
class Exception: public std::runtime_error {
public:

    Exception(const char* str):
        std::runtime_error(str) {
    }

    Exception(const std::string& str):
        std::runtime_error(str) {
    }
};


}
