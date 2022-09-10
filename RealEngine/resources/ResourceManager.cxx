/*! 
 *  @author    Dubsky Tomas
 */
module RealEngine.resources.ResourceManager;


namespace RE {

SharedTexture ResourceManager::texture(const TextureSeed& seed) {
    return s_textureCache.texture(seed.toFullPath());
}

SharedTexture ResourceManager::texture(const std::string& filePathPNG) {
    return s_textureCache.texture(filePathPNG);
}

}