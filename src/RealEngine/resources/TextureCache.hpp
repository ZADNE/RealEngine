/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <memory>
#include <unordered_map>

#include <RealEngine/graphics/textures/TextureShaped.hpp>
#include <RealEngine/resources/PackageLoader.hpp>
#include <RealEngine/resources/ResourceID.hpp>

namespace re {

/**
 * @brief Is a texture that can be shared across Rooms
 */
using SharedTexture = std::shared_ptr<TextureShaped>;

class TextureCache {
public:
    explicit TextureCache(const PackageLoader& packageLoader);

    /**
     * @brief Gets texture as a shared resource.
     * @return TextureShaped as a shared resource
     */
    SharedTexture texture(ResourceID id);

private:
    const PackageLoader& m_packageLoader;
    std::unordered_map<ResourceID::IDType, std::weak_ptr<TextureShaped>> m_textureMap;
};

} // namespace re
