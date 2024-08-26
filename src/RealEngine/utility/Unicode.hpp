/*!
 *  @author    Dubsky Tomas
 *  @file      A minimalistic set of functions concerned with UNICODE
 */
#pragma once
#include <array>
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
 * @brief Denotes a range of Unicode characters
 */
struct UnicodeRange {
    char32_t firstChar{}; /**< Inclusive */
    char32_t lastChar{};  /**< Inclusive */
};

/**
 * @brief Range of ASCII characters that have printable glyph assigned
 */
constexpr std::array k_asciiPrintableUnicodeRanges =
    std::to_array<UnicodeRange>({{.firstChar = U' ', .lastChar = U'~'}});

/**
 * @brief Range of czech unicode characters
 */
constexpr std::array k_czechUnicodeRanges =
    std::to_array<UnicodeRange>({{.firstChar = ' ', .lastChar = U'ž'}});

/**
 * @brief Counts number of bytes representing the first character
 */
constexpr int codeSize(std::u8string_view str) {
    uint8_t c = str[0] & 0b1111'0000;
    int ones  = std::countl_one(c);
    return ones + static_cast<int>(!ones);
}

/**
 * @brief Reads first unicode character from the string
 */
constexpr char32_t readCode(std::u8string_view& str) {
    if (str.empty()) {
        return U'\0';
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
