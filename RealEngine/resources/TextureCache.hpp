/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <unordered_map>
#include <memory>

#include <RealEngine/rendering/textures/Texture.hpp>

namespace RE {

/**
 * @brief Is a Texture that can be shared across Rooms
*/
using SharedTexture = std::shared_ptr<Texture>;

class TextureCache {
public:

    TextureCache() {}

    /**
     * @brief Gets texture as a shared resource.
     * @param filePathPNG Path to the PNG
     * @return Texture as a shared resource
    */
    SharedTexture texture(const std::string& filePathPNG);

private:

    std::unordered_map<std::string, std::weak_ptr<Texture>> m_textureMap;
};

}