﻿/**
 *  @author    Dubsky Tomas
 */
#include <RealEngine/resources/ResourceLoader.hpp>
#include <RealEngine/utility/BuildType.hpp>

namespace re {

template<>
DataResource ResourceLoader::load<DataResource>(ResourceID id) const {
    if constexpr (k_buildType == BuildType::Debug) {
        // Debug: Load the file directly
        return readBinaryFile(id.path());
    } else {
        // Release: Extract from package
        std::vector<unsigned char> rval;
        m_inputArchive.extractTo(rval, id);
        return rval;
    }
}

template<>
TextureShaped ResourceLoader::load<TextureShaped>(ResourceID id) const {
    return TextureShaped{PNGLoader::load(load<DataResource>(id))};
}

} // namespace re
