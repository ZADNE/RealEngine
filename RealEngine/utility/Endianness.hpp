/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <algorithm>
#include <array>
#include <climits>
#include <cstdint>

#if CHAR_BIT != 8
#    error 8 bits per byte required
#endif // CHAR_BIT != 8

namespace re {

/**
 * @brief Checks at runtime whether the system uses big endian
 * or little endian byte order.
 * @return True if the system is big endian, false otherwise
 */
bool isSystemBigEndian() {
    union {
        uint32_t i;
        uint8_t  c[4];
    } u = {0x01020304};

    return (u.c[0] == 1);
}

/**
 * @brief Reverses order of bytes in given value
 * @tparam T Any type
 * @param val Value to be reversed
 * @return Value with reversed bytes
 */
template<typename T>
constexpr T reverseByteOrder(const T& val) {
    union U {
        U()
            : raw() {}
        T                                val;
        std::array<std::byte, sizeof(T)> raw;
    };
    U source;
    source.val = val;
    U dest;

    std::reverse_copy(source.raw.begin(), source.raw.end(), dest.raw.begin());
    return dest.val;
}

/**
 * @brief Converts given value in network byte order to host byte order
 * @tparam T Any type
 * @param net Value in network byte order
 * @return Value in host byte order
 */
template<typename T>
constexpr T ntoh(const T& net) {
    if (isSystemBigEndian()) { // Big endian
        return net;
    } else { // Little endian
        return reverseByteOrder(net);
    }
}

/**
 * @brief Converts given value in host byte order to network byte order
 * @tparam T Any type
 * @param net Value in host byte order
 * @return Value in network byte order
 */
template<typename T>
constexpr T hton(const T& host) {
    if (isSystemBigEndian()) { // Big endian
        return host;
    } else { // Little endian
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
    if (!isSystemBigEndian()) { // Little endian
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
    if (!isSystemBigEndian()) { // Little endian
        (reverseByteOrder(host), ...);
    }
}

} // namespace re