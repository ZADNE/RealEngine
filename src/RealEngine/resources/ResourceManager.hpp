/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/resources/PackageLoader.hpp>
#include <RealEngine/resources/TextureCache.hpp>

namespace re {

/**
 * @brief Ensures that there is at most one copy of shared resources.
 *
 * Resources managed by resource manager: textures (only so far).
 * Resources are released once there are no references to them.
 *
 * Also accessible through "RM" abbreviation.
 */
class ResourceManager {
public:
    /**
     * @copydoc TextureCache::texture
     */
    static SharedTexture texture(ResourceID id);

    /**
     * @brief Creates an unmanaged texture.
     * @warning Calling this function multiple times creates
     *          multiple copies of the texture
     */
    static TextureShaped textureUnmanaged(ResourceID id);

private:
    static inline PackageLoader s_packageLoader{};
    static inline TextureCache s_textureCache{s_packageLoader};
};

/**
 * @brief Is an abbreviation type alias for ResourceManager
 */
using RM = class ResourceManager;

} // namespace re
