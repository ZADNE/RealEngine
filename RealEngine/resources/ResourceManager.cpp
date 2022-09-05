/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/resources/ResourceManager.hpp>

namespace RE {

SharedTexture ResourceManager::texture(const std::string& filePathNoExt) {
    return s_textureCache.texture(filePathNoExt);
}

}