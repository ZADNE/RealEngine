/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/resources/TextureCache.hpp>

namespace re {

SharedTexture TextureCache::texture(const std::string& filePathPNG) {
    auto mapIterator = m_textureMap.find(filePathPNG);
    SharedTexture stored;
    if (mapIterator != m_textureMap.end() && (stored = mapIterator->second.lock())) {
        return stored;
    } else {//Texture never accessed before or it has expired
        auto made = std::make_shared<TextureShaped>(filePathPNG);
        m_textureMap.insert_or_assign(filePathPNG, made);
        return made;
    }
}

}