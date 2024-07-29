/*!
 *  @author    Dubsky Tomas
 *  @file      A minimalistic set of functions concerned with UNICODE
 */
#pragma once
#include <bit>
#include <string_view>

#include <RealEngine/utility/BuildType.hpp>
#include <RealEngine/utility/Error.hpp>

namespace re {

/**
 * @brief Not a character code
 */
constexpr char32_t k_invalidCode = 0xffff;

/**
 * @brief Counts number of bytes representing the first character
 */
constexpr int codeSize(std::u8string_view str) {
    uint8_t c = str[0] & 0b11110000;
    int ones  = std::countl_one(c);
    return ones + static_cast<int>(!ones);
}

/**
 * @brief Reads first unicode character from the string
 */
constexpr char32_t readCode(std::u8string_view& str) {
    if (str.empty()) {
        return 0x0000;
    }

    const int size = codeSize(str);
    if (size > str.size()) {
        return k_invalidCode; // Not all bytes of codepoint provided
    }

    // Extract bits from first byte
    char8_t mask0    = (1 << (7 - (size - static_cast<int>(!(size - 1))))) - 1;
    char32_t unicode = str[0] & mask0;
    str.remove_prefix(1);

    // Extract bits from remaining bytes
    constexpr char8_t k_mask = 0b0011'1111;
    for (int i = 1; i < size; ++i) {
        if ((str[0] & ~k_mask) != 0b1000'0000) {
            return k_invalidCode; // High bits are not correct
        }
        unicode = (unicode << 6) | (str[0] & k_mask);
        str.remove_prefix(1);
    }

    return unicode;
}

} // namespace re
