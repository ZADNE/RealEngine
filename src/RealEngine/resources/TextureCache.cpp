/**
 *  @author    Dubsky Tomas
 */
#include <RealEngine/resources/TextureCache.hpp>

namespace re {

TextureCache::TextureCache(const ResourceLoader& resourceLoader)
    : m_resourceLoader{resourceLoader} {
}

SharedTexture TextureCache::texture(ResourceID id) {
    auto it = m_textureMap.find(id);
    SharedTexture stored;
    if (it != m_textureMap.end() && (stored = it->second.lock())) {
        // Texture present
        return stored;
    } else {
        // Texture never accessed before or it has expired
        auto encodedPNG = m_resourceLoader.load(id);
        auto tex = std::make_shared<TextureShaped>(PNGLoader::load(encodedPNG));
        m_textureMap.insert_or_assign(id, tex);
        return tex;
    }
}

} // namespace re
