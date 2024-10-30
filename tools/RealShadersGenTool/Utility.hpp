/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <concepts>
#include <cstdio>
#include <filesystem>
#include <print>
#include <utility>

namespace rsg {

/**
 * @brief Prints a message and exits unsuccessfully
 */
template<class... Args>
[[noreturn]] void fatalError(std::format_string<Args...>&& formatString, Args&&... args) {
    std::println<Args...>(
        stderr, std::forward<std::format_string<Args...>>(formatString),
        std::forward<Args>(args)...
    );
    std::exit(1);
}

/**
 * @brief Reads whole text file into a string
 */
std::string readWholeFile(const std::filesystem::path& fullPath);

/**
 * @brief Divides two unsigned numbers and rounds them towards positive infinity
 */
template<std::unsigned_integral T, std::convertible_to<T> TConv>
constexpr T ceilDiv(T dividend, TConv divisor) {
    return (dividend + divisor - T{1}) / divisor;
}

/**
 * @brief Rounds a number to the next greater-or-equal multiple of another number
 */
template<std::unsigned_integral T, std::convertible_to<T> TConv>
constexpr T roundToMultiple(T toRound, TConv multipleOf) {
    return ceilDiv(toRound, multipleOf) * multipleOf;
}

} // namespace rsg
