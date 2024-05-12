/*!
 *  @author    Dubsky Tomas
 *  @file
 */
#pragma once

namespace re {

enum class BuildType {
    Debug,
    Release
};

/**
 * @brief Represents the current build type (Debug/Release)
 */
constexpr auto k_buildType =
#ifndef NDEBUG
    BuildType::Debug
#else
    BuildType::Release
#endif // !NDEBUG
    ;

} // namespace re
