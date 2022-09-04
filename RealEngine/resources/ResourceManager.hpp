/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/resources/TextureCache.hpp>


namespace RE {

/**
 * @brief Ensures there is at most one copy of shared resources.
 *
 * Resources managed by resource manager: textures, shader programs.
 * Resources are released once there are no references to them.
 * 
 * Also accessible through "RM" abbreviation.
*/
template<RE::Renderer R>
class ResourceManager {
public:

    /**
     * @copydoc TextureCache::texture
    */
    static TexturePtr texture(const std::string& filePath);

private:

    static inline TextureCache<R> s_textureCache;
};

/**
 * @brief Is abbreviation for ResourceManager
*/
template<RE::Renderer R>
using RM = class ResourceManager<R>;

}