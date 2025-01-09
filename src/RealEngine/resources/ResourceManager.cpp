/**
 *  @author    Dubsky Tomas
 */
#include <RealEngine/resources/ResourceManager.hpp>

namespace re {

SharedTextureShaped ResourceManager::texture(ResourceID id) {
    return s_resourceCache.resource<TextureShaped>(id);
}

TextureShaped ResourceManager::textureUnmanaged(ResourceID id) {
    return s_resourceLoader.load<TextureShaped>(id);
}

SharedDataResource ResourceManager::data(ResourceID id) {
    return s_resourceCache.resource<DataResource>(id);
}

std::vector<unsigned char> ResourceManager::dataUnmanaged(ResourceID id) {
    return s_resourceLoader.load<std::vector<unsigned char>>(id);
}

} // namespace re
