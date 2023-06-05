/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/resources/ResourceManager.hpp>

namespace re {

SharedTexture ResourceManager::texture(const TextureSeed& seed) {
    return s_textureCache.texture(seed.toFullPath());
}

SharedTexture ResourceManager::texture(const std::string& filePathPNG) {
    return s_textureCache.texture(filePathPNG);
}

} // namespace re