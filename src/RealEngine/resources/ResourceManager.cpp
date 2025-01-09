/**
 *  @author    Dubsky Tomas
 */
#include <RealEngine/resources/ResourceManager.hpp>

namespace re {

SharedTexture ResourceManager::texture(ResourceID id) {
    return s_textureCache.texture(id);
}

TextureShaped ResourceManager::textureUnmanaged(ResourceID id) {
    auto encodedPNG = s_resourceLoader.load(id);
    return TextureShaped{PNGLoader::load(encodedPNG)};
}

std::vector<unsigned char> ResourceManager::dataUnmanaged(ResourceID id) {
    return s_resourceLoader.load(id);
}

} // namespace re
