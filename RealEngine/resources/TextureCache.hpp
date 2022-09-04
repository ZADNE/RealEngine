/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <unordered_map>
#include <memory>

#include <RealEngine/rendering/textures/Texture.hpp>

namespace RE {


template<RE::Renderer R>
using ShaderTexture = std::shared_ptr<const Texture<R>>;

template<RE::Renderer R>
class TextureCache {
public:

    TextureCache() {}

    /**
     * @brief Gets texture as a shared resource.
     * @param filePathNoExt Path to the texture (without extension)
     * @return Texture as a shared resource
    */
    ShaderTexture<R> texture(const std::string& filePathNoExt);

private:

    std::unordered_map<std::string, std::weak_ptr<Texture<RendererLateBind>>> m_textureMap;
    std::string m_textureFolder = "textures/";
};

}