/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/utility/BuildType.hpp>

namespace re {

constexpr const char* k_packageKey  = "906def63237d";
constexpr const char* k_packageName = "package.dat";

constexpr const char* default7ZipSharedLibLocation() {
    switch (k_buildOS) {
    case BuildOS::Windows: return "7z.dll";
    case BuildOS::Linux:   return "/usr/lib/7zip/7z.so";
    default:               return nullptr;
    }
}

} // namespace re
