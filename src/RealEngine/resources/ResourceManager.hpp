/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/resources/TextureCache.hpp>

namespace re {

/**
 * @brief Ensures that there is at most one copy of shared resources.
 *
 * Resources managed by resource manager: textures, shader programs.
 * Resources are released once there are no references to them.
 *
 * Also accessible through "RM" abbreviation.
 */
class ResourceManager {
public:
    /**
     * @copydoc TextureCache::texture
     */
    static SharedTexture texture(const TextureSeed& seed);

    /**
     * @copydoc TextureCache::texture
     */
    static SharedTexture texture(const std::string& filePathPNG);

private:
    static inline TextureCache s_textureCache{};
};

/**
 * @brief Is an abbreviation type alias for ResourceManager
 */
using RM = class ResourceManager;

} // namespace re
