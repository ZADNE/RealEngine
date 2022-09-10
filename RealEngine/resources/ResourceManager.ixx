/*! 
 *  @author    Dubsky Tomas
 */
export module RealEngine.resources.ResourceManager;
import RealEngine.resources.TextureCache;


export namespace RE {

/**
 * @brief Ensures there is at most one copy of shared resources.
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
 * @brief Is abbreviation for ResourceManager
*/
using RM = class ResourceManager;

}