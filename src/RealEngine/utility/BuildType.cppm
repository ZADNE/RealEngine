/**
 *  @author    Dubsky Tomas
 *  @file
 */
export module realengine:utility.buildtype;

export namespace re {

#if !defined(RE_BUILDING_FOR_DEBUG) || !defined(RE_BUILDING_FOR_RELEASE)
#    error "RE_BUILDING_FOR_* macros must be defined by CMake."
#endif
static_assert(RE_BUILDING_FOR_DEBUG + RE_BUILDING_FOR_RELEASE == 1);

/**
 * @brief Supported build configuration types
 */
enum class BuildType {
    Debug,
    Release
};

/**
 * @brief Holds the current build type (Debug/Release)
 */
constexpr BuildType k_buildType
#if defined(__DOXYGEN__)
    = <build configuration dependent>;
#elif RE_BUILDING_FOR_DEBUG
    = BuildType::Debug;
#elif RE_BUILDING_FOR_RELEASE
    = BuildType::Release;
#else
#    error "Unknown build type"
#endif

#if !defined(RE_BUILDING_FOR_WINDOWS) || !defined(RE_BUILDING_FOR_LINUX)
#    error "RE_BUILDING_FOR_* macros must be defined by CMake."
#endif
static_assert(RE_BUILDING_FOR_WINDOWS + RE_BUILDING_FOR_LINUX == 1);

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
constexpr BuildOS k_buildOS
#if defined(__DOXYGEN__)
    = <build configuration dependent>;
#elif RE_BUILDING_FOR_WINDOWS
    = BuildOS::Windows;
#elif RE_BUILDING_FOR_LINUX
    = BuildOS::Linux;
#else
#    error "Unknown build OS"
#endif

} // namespace re
