/**
 *  @author    Dubsky Tomas
 */
#include <RealEngine/resources/TextureCache.hpp>

namespace re {

TextureCache::TextureCache(const PackageLoader& packageLoader)
    : m_packageLoader{packageLoader} {
}

SharedTexture TextureCache::texture(ResourceID id) {
    auto it = m_textureMap.find(id);
    SharedTexture stored;
    if (it != m_textureMap.end() && (stored = it->second.lock())) {
        return stored; // Texture present
    } else {           // Texture never accessed before or it has expired
        auto encodedPNG = m_packageLoader.unpack(id);
        auto tex = std::make_shared<TextureShaped>(PNGLoader::load(encodedPNG));
        m_textureMap.insert_or_assign(id, tex);
        return tex;
    }
}

} // namespace re
