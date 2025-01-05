/**
 *  @author    Dubsky Tomas
 */
#include <RealEngine/resources/ResourceManager.hpp>

namespace re {

SharedTexture ResourceManager::texture(ResourceID id) {
    return s_textureCache.texture(id);
}

TextureShaped ResourceManager::textureUnmanaged(ResourceID id) {
    auto encodedPNG = s_packageLoader.unpack(id);
    return TextureShaped{PNGLoader::load(encodedPNG)};
}

} // namespace re
