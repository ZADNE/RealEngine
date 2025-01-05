/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>
#include <vector>

#include <bit7z/bitmemextractor.hpp>
// bit7z may include Windows.h which introduces macros...
#ifdef MemoryBarrier
#    undef MemoryBarrier
#endif

#include <RealEngine/resources/FileIO.hpp>
#include <RealEngine/resources/ResourceID.hpp>

namespace re {

class PackageLoader {
public:
    constexpr static const char* k_packageKey  = "906def63237d";
    constexpr static const char* k_packageName = "package.dat";

    PackageLoader() = default;

    std::vector<unsigned char> unpack(ResourceID id) const;

private:
    std::vector<unsigned char> m_compressedPackage = readBinaryFile(k_packageName);
    bit7z::Bit7zLibrary m_lib{};
    bit7z::BitMemExtractor m_extractor{m_lib, bit7z::BitFormat::SevenZip};
    [[no_unique_address]] struct Empty {       // A hack to set password before
    } _empty = [&]() {                         // m_inputArchive is constructed.
        m_extractor.setPassword(k_packageKey); // BitMemExtractor has no constructor
        return Empty{};                        // that would set the password.
    }();
    bit7z::BitInputArchive m_inputArchive{m_extractor, m_compressedPackage};
};

} // namespace re
