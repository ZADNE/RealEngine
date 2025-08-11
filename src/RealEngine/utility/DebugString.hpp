/**
 *  @author    Dubsky Tomas
 *  @file
 */
#pragma once
#include <RealEngine/utility/BuildType.hpp>

namespace re {

/**
 * @brief Holds a string in debug build but is an empty type in release build
 */
template<BuildType buildType = k_buildType>
class DebugString;

/**
 * @brief Holds the debug string
 */
template<>
class DebugString<BuildType::Debug> {
public:
    constexpr DebugString() {}
    constexpr DebugString(const char* name)
        : m_name(name) {}

    constexpr operator const char*() const { return m_name; }

private:
    const char* m_name{};
};

/**
 * @brief Is an empty type but keeps the same API as the debug version
 */
template<>
class DebugString<BuildType::Release> {
public:
    constexpr DebugString() {}
    constexpr DebugString([[maybe_unused]] const char* name_) {}

    constexpr operator const char*() const { return ""; }
};

} // namespace re
