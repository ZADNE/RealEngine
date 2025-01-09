/**
 *  @author    Horu / sgf4 (DEV Community: Strings as template parameters)
 */
#pragma once
#include <algorithm>

namespace re {

/**
 * @brief   Allows C string literals to be used as template parameters.
 * @tparam  N Length of the string. This should be deduced.
 * @author  Horu / sgf4 (DEV Community: Strings as template parameters)
 */
template<std::size_t N>
struct CompTimeString {
    char data[N]{};

    consteval CompTimeString(const char (&str)[N]) {
        std::copy_n(str, N, data);
    }

    consteval bool operator==(const CompTimeString<N> str) const {
        return std::equal(str.data, str.data + N, data);
    }

    template<std::size_t N2>
    consteval bool operator==(const CompTimeString<N2> s) const {
        return false;
    }

    template<std::size_t N2>
    consteval CompTimeString<N + N2 - 1> operator+(const CompTimeString<N2> str
    ) const {
        char newchar[N + N2 - 1]{};
        std::copy_n(data, N - 1, newchar);
        std::copy_n(str.data, N2, newchar + N - 1);
        return newchar;
    }

    consteval char operator[](std::size_t n) const { return data[n]; }

    consteval std::size_t size() const { return N - 1; }
};

template<std::size_t s1, std::size_t s2>
consteval auto operator+(CompTimeString<s1> fs, const char (&str)[s2]) {
    return fs + CompTimeString<s2>(str);
}

template<std::size_t s1, std::size_t s2>
consteval auto operator+(const char (&str)[s2], CompTimeString<s1> fs) {
    return CompTimeString<s2>(str) + fs;
}

template<std::size_t s1, std::size_t s2>
consteval auto operator==(CompTimeString<s1> fs, const char (&str)[s2]) {
    return fs == CompTimeString<s2>(str);
}

template<std::size_t s1, std::size_t s2>
consteval auto operator==(const char (&str)[s2], CompTimeString<s1> fs) {
    return CompTimeString<s2>(str) == fs;
}

} // namespace re
