/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/resources/TextureCache.hpp>

namespace RE {

TextureUnion::TextureUnion(const std::string& filePathPNG) :
    m_lateBindTex(filePathPNG, Texture<RendererLateBind>::DEFAULT_PARAMETERS) {}

TextureUnion::~TextureUnion() {
    m_lateBindTex.~Texture();
}

SharedTexture TextureCache::texture(const std::string& filePathPNG) {
    auto mapIterator = m_textureMap.find(filePathPNG);
    SharedTexture stored;
    if (mapIterator != m_textureMap.end() && (stored = mapIterator->second.lock())) {
        return stored;
    } else {//Texture never accessed before or it has expired
        auto made = std::make_shared<TextureUnion>(filePathPNG);
        m_textureMap.insert_or_assign(filePathPNG, made);
        return made;
    }
}

}