/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <concepts>

namespace re {

/**
 * @brief Divides two unsigned numbers and rounds them towards positive infinity
 */
template<std::unsigned_integral T, std::convertible_to<T> TConv>
constexpr T ceilDiv(T dividend, TConv divisor) {
    return (dividend + divisor - T{1}) / divisor;
}

} // namespace re
