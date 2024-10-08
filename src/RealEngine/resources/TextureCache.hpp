﻿/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <memory>
#include <unordered_map>

#include <RealEngine/graphics/textures/TextureShaped.hpp>

namespace re {

/**
 * @brief Is a texture that can be shared across Rooms
 */
using SharedTexture = std::shared_ptr<TextureShaped>;

class TextureCache {
public:
    TextureCache() {}

    /**
     * @brief Gets texture as a shared resource.
     * @param filePathPNG Path to the PNG
     * @return TextureShaped as a shared resource
     */
    SharedTexture texture(const std::string& filePathPNG);

private:
    std::unordered_map<std::string, std::weak_ptr<TextureShaped>> m_textureMap;
};

} // namespace re
