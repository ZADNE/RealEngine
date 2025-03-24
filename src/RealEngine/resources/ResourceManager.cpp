/**
 *  @author    Dubsky Tomas
 */
#include <RealEngine/resources/ResourceManager.hpp>

namespace re {

std::shared_ptr<TextureShaped> ResourceManager::texture(ResourceID id) {
    return s_resourceCache.resource<TextureShaped>(id);
}

TextureShaped ResourceManager::textureUnmanaged(ResourceID id) {
    return s_resourceLoader.load<TextureShaped>(id);
}

std::shared_ptr<DataResource> ResourceManager::data(ResourceID id) {
    return s_resourceCache.resource<DataResource>(id);
}

std::vector<unsigned char> ResourceManager::dataUnmanaged(ResourceID id) {
    return s_resourceLoader.load<std::vector<unsigned char>>(id);
}

} // namespace re
