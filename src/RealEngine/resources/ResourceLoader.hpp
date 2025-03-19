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

#include <RealEngine/graphics/textures/TextureShaped.hpp>
#include <RealEngine/resources/FileIO.hpp>
#include <RealEngine/resources/PackageConstants.hpp>
#include <RealEngine/resources/ResourceID.hpp>

namespace re {

namespace details {
template<typename T, typename... U>
concept IsAnyOf = (std::same_as<T, U> || ...);
}

/**
 * @brief Is an alias for vector of bytes...
 */
using DataResource = std::vector<unsigned char>;

/**
 * @brief Must be one of: re::DataResource, re::TextureShaped
 */
template<typename T>
concept IsResource = details::IsAnyOf<T, DataResource, TextureShaped>;

/**
 * @brief   Loads files from compressed 'encrypted' package.
 * @details Resource more less means a file in this context.
 * @details In debug build, reads the original files instead to allow quick
 *          iteration without needing to run rerun packaging everytime.
 * @see     ResourcePackager executable (CMake target)
 */
class ResourceLoader {
public:
    ResourceLoader() = default;

    /**
     * @brief Loads resource from package (Release) or directly from file (Debug)
     * @tparam T Any resource type
     */
    template<typename T>
        requires IsResource<T>
    T load(ResourceID id) const;

private:
#ifdef NDEBUG
    std::vector<unsigned char> m_compressedPackage = readBinaryFile(k_packageName);
    bit7z::Bit7zLibrary m_lib{
        k_buildOS == BuildOS::Windows ? "7z.dll" : "/usr/lib/7zip/7z.so"
    };
    bit7z::BitMemExtractor m_extractor{m_lib, bit7z::BitFormat::SevenZip};
    [[no_unique_address]] struct Empty {       // A hack to set password before
    } _empty = [&]() {                         // m_inputArchive is constructed.
        m_extractor.setPassword(k_packageKey); // BitMemExtractor has no constructor
        return Empty{};                        // that would set the password.
    }();
    bit7z::BitInputArchive m_inputArchive{m_extractor, m_compressedPackage};
#endif
};

} // namespace re
