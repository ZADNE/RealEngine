/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/resources/TextureCache.hpp>

namespace RE {

TextureUnion::TextureUnion(const std::string& filePathPNG) :
    m_lateBindTex(filePathPNG) {}

TextureUnion::~TextureUnion() {
    m_lateBindTex.~Texture();
}

SharedTexture TextureCache::texture(const std::string& filePathNoExt) {
    auto mapIterator = m_textureMap.find(filePathNoExt);
    SharedTexture stored;
    if (mapIterator != m_textureMap.end() && (stored = mapIterator->second.lock())) {
        return stored;
    } else {//Texture never accessed before or it has expired
        auto made = std::make_shared<TextureUnion>(m_textureFolder + filePathNoExt + ".png");
        m_textureMap.insert_or_assign(filePathNoExt, made);
        return made;
    }
}

}