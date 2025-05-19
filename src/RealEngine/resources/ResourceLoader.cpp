/**
 *  @author    Dubsky Tomas
 */
#include <RealEngine/resources/ResourceLoader.hpp>
#include <RealEngine/utility/BuildType.hpp>

namespace re {

template<>
DataResource ResourceLoader::load<DataResource>(ResourceID id) const {
#if RE_BUILDING_FOR_DEBUG
    // Load the file directly
    return readBinaryFile(id.path());
#elif RE_BUILDING_FOR_RELEASE // ^^^ RE_BUILDING_FOR_DEBUG
    // Extract from package
    std::vector<unsigned char> rval;
    m_inputArchive.extractTo(rval, id);
    return rval;
#endif                        // RE_BUILDING_FOR_RELEASE
}

template<>
TextureShaped ResourceLoader::load<TextureShaped>(ResourceID id) const {
    return TextureShaped{PNGLoader::load(load<DataResource>(id))};
}

} // namespace re
