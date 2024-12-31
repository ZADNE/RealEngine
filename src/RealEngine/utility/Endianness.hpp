/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <algorithm>
#include <array>
#include <bit>

namespace re {

/**
 * @brief Checks at compile-time whether the system uses big endian byte order
 */
constexpr bool isSystemBigEndian() {
    return (std::endian::native == std::endian::big);
}

/**
 * @brief Reverses order of bytes in given value
 * @tparam T Any type
 * @param val Value to be reversed
 * @return Value with reversed bytes
 * @note Almost the same as std::byteswap but allows non-integral types too.
 */
template<typename T>
constexpr T reverseByteOrder(const T& val) {
    auto bytes = std::bit_cast<std::array<std::byte, sizeof(T)>>(val);
    std::ranges::reverse(bytes);
    return std::bit_cast<T>(bytes);
}

/**
 * @brief Converts given value in network byte order to host byte order
 * @tparam T Any type
 * @param net Value in network byte order
 * @return Value in host byte order
 */
template<typename T>
constexpr T ntoh(const T& net) {
    if constexpr (isSystemBigEndian()) { // Big endian
        return net;
    } else {                             // Little endian
        return reverseByteOrder(net);
    }
}

/**
 * @brief Converts given value in host byte order to network byte order
 * @tparam T Any type
 * @param host Value in host byte order
 * @return Value in network byte order
 */
template<typename T>
constexpr T hton(const T& host) {
    if constexpr (isSystemBigEndian()) { // Big endian
        return host;
    } else {                             // Little endian
        return reverseByteOrder(host);
    }
}

/**
 * @brief Converts given values in network byte order to host byte order in bulk.
 *
 * This is faster than consecutive call to ntoh().
 *
 * @tparam ...Args Any types
 * @param ...net Values in network byte order
 */
template<typename... Args>
constexpr void ntohBulk(Args&... net) {
    if constexpr (!isSystemBigEndian()) { // Little endian
        (reverseByteOrder(net), ...);
    }
}

/**
 * @brief Converts given values in host byte order to network byte order in bulk.
 *
 * This is faster than consecutive call to hton().
 *
 * @tparam ...Args Any types
 * @param ...host Values in host byte order
 */
template<typename... Args>
constexpr void htonBulk(Args&... host) {
    if constexpr (!isSystemBigEndian()) { // Little endian
        (reverseByteOrder(host), ...);
    }
}

} // namespace re
