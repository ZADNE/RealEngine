/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/resources/ResourceLoader.hpp>
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
     *          multiple copies of the texture.
     */
    static TextureShaped textureUnmanaged(ResourceID id);

    /**
     * @brief Loads an unmanaged data.
     * @warning Calling this function multiple times creates
     *          multiple copies of the data.
     */
    static std::vector<unsigned char> dataUnmanaged(ResourceID id);

private:
    static inline ResourceLoader s_resourceLoader{};
    static inline TextureCache s_textureCache{s_resourceLoader};
};

/**
 * @brief Is an abbreviation type alias for ResourceManager
 */
using RM = class ResourceManager;

} // namespace re
