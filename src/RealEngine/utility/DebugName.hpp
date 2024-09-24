/**
 *  @author    Dubsky Tomas
 *  @file
 */
#pragma once
#include <string_view>

#include <RealEngine/utility/BuildType.hpp>

namespace re {

/**
 * @brief Holds a string name in debug build but is an empty type in release build
 */
template<BuildType buildType = k_buildType>
class DebugName;

/**
 * @brief Holds the debug name
 */
template<>
class DebugName<BuildType::Debug> {
public:
    constexpr DebugName() {}
    constexpr DebugName(const char* name)
        : m_name(name) {}
    constexpr DebugName(std::string_view name)
        : m_name(name) {}

    constexpr operator const char*() const { return m_name.data(); }

private:
    std::string_view m_name;
};

/**
 * @brief Is an empty type but keeps the same API as the debug version
 */
template<>
class DebugName<BuildType::Release> {
public:
    constexpr DebugName() {}
    constexpr DebugName([[maybe_unused]] const char* name_) {}
    constexpr DebugName([[maybe_unused]] std::string_view name_) {}

    constexpr operator const char*() const { return ""; }
};

} // namespace re
