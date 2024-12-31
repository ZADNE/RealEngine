/**
 *  @author    Dubsky Tomas
 */
#include <RealEngine/resources/PackageLoader.hpp>

namespace re {

void PackageLoader::loadFile(const std::string& path) const {
    bit7z::BitFileExtractor extractor{m_lib, bit7z::BitFormat::SevenZip};
    extractor.setPassword(k_packageKey);

    std::vector<bit7z::byte_t> buffer;
    extractor.extractMatching(k_packageName, path, buffer);

    // TODO
}

} // namespace re
