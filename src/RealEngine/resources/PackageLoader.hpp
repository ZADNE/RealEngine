/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <bit7z/bitfileextractor.hpp>

namespace re {

class PackageLoader {
public:
    constexpr static const char* k_packageKey  = "906def63237d";
    constexpr static const char* k_packageName = "package.dat";

    PackageLoader() = default;

    void loadFile(const std::string& path) const;

private:
    bit7z::Bit7zLibrary m_lib{};
};

} // namespace re
