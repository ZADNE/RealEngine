/**
 *  @author    Dubsky Tomas
 */
#include <RealEngine/resources/ResourceLoader.hpp>
#include <RealEngine/utility/BuildType.hpp>

namespace re {

template<>
DataResource ResourceLoader::load<DataResource>(ResourceID id) const {
    // Prefer unpackaged data in debug build
    if constexpr (k_buildType == BuildType::Debug) {
        if (std::filesystem::exists(id.path())) {
            return readBinaryFile(id.path());
        }
    }

    // Extract from package
    std::vector<unsigned char> rval;
    m_inputArchive.extractTo(rval, id);
    return rval;
}

template<>
TextureShaped ResourceLoader::load<TextureShaped>(ResourceID id) const {
    return TextureShaped{PNGLoader::load(load<DataResource>(id))};
}

template DataResource ResourceLoader::load<DataResource>(ResourceID id) const;
template TextureShaped ResourceLoader::load<TextureShaped>(ResourceID id) const;

} // namespace re
