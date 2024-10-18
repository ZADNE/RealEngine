/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <cstdio>
#include <print>

namespace rsg {

template<class... Args>
[[noreturn]] void fatalError(Args... args) {
    std::println(stderr, args...);
    std::exit(1);
}

} // namespace rsg
