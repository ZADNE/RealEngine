/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>

namespace re {

/**
 * @brief Provides info required by running RealEngine application to reload data
 */
struct HotReloadInitInfo {
    const char* cmakePath{};       ///< Path to CMake executable
    const char* targetName{};      ///< Name of CMake target
    const char* targetSourceDir{}; ///< Path to root of target's source directory
    const char* binaryDir{};       ///< Path to project's binary directory
};

} // namespace re
