/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <cstdio>
#include <filesystem>
#include <print>
#include <utility>

namespace rsg {

template<class... Args>
[[noreturn]] void fatalError(std::format_string<Args...>&& formatString, Args&&... args) {
    std::println<Args...>(
        stderr, std::forward<std::format_string<Args...>>(formatString),
        std::forward<Args>(args)...
    );
    std::exit(1);
}

std::string readWholeFile(const std::filesystem::path& fullPath);

} // namespace rsg
