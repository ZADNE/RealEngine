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
#define RE_BUILDING_FOR_DEBUG 1
#define RE_BUILDING_FOR_RELEASE 1
#elif !defined(NDEBUG)
constexpr BuildType k_buildType = BuildType::Debug;
#define RE_BUILDING_FOR_DEBUG 1
#define RE_BUILDING_FOR_RELEASE 0
#else
constexpr BuildType k_buildType = BuildType::Release;
#define RE_BUILDING_FOR_DEBUG 0
#define RE_BUILDING_FOR_RELEASE 1
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
