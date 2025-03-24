/**
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
 * @brief Holds the current build type (Debug/Release)
 */
#if defined(__DOXYGEN__)
constexpr BuildType k_buildType = <build configuration dependent>;
#elif !defined(NDEBUG)
constexpr BuildType k_buildType = BuildType::Debug;
#else
constexpr BuildType k_buildType = BuildType::Release;
#endif

/**
 * @brief The types of operating systems that RealEngine can be built for
 * @note Let's not get into versions/distributions/details, shall we?
 */
enum class BuildOS {
    Windows,
    Linux
};

/**
 * @brief Holds the build operating system (Windows/Linux)
 */
#if defined(__DOXYGEN__)
constexpr BuildOS k_buildOS = <build configuration dependent>;
#elif defined(_WIN32)
constexpr BuildOS k_buildOS = BuildOS::Windows;
#elif defined(__linux__)
constexpr BuildOS k_buildOS = BuildOS::Linux;
#else
#    error "Unsupported or undetected operating system"
#endif

} // namespace re
