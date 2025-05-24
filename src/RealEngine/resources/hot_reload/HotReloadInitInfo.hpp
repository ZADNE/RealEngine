/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>

namespace re {

/**
 * @brief Provides info required by a running RealEngine application to reload data
 */
struct HotReloadInitInfo {
    const char* cmakePath{};            ///< Path to CMake executable
    const char* sourceDir{};            ///< Path to project's source directory
    const char* binaryDir{};            ///< Path to project's binary directory
    const char* shaderFileExtensions{}; ///< CMake-style semicolon-separated list
    const char* targetName{};           ///< Name of CMake target
    const char* targetBaseDirRel{};     ///< Relative path to target's base dir
};

} // namespace re
