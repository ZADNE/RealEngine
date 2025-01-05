﻿/**
 *  @author    Dubsky Tomas
 */
#include <format>

#include <RealEngine/resources/PackageLoader.hpp>
#include <RealEngine/utility/BuildType.hpp>

namespace re {

std::vector<unsigned char> PackageLoader::unpack(ResourceID id) const {
    // Prefer unpackaged data in debug build
    if constexpr (/*k_buildType == BuildType::Debug*/ false) {
        if (std::filesystem::exists(id.path())) {
            return readBinaryFile(id.path());
        }
    }

    // Extract from package
    std::vector<unsigned char> rval;
    m_inputArchive.extractTo(rval, id);
    return rval;
}

} // namespace re
