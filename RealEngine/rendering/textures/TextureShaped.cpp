﻿/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/textures/TextureShaped.hpp>

namespace RE {

TextureShaped::TextureShaped(PNGLoader::PNGData&& pngData) :
    Texture(TextureCreateInfo{
        .extent = vk::Extent3D{pngData.dims.x, pngData.dims.y, 1u},
        .texels = std::move(pngData.texels)
    }),
    m_shape(TextureShape{
        .subimageDims = pngData.shape.subimageDims == glm::vec2{0.0f, 0.0f} ? glm::vec2{pngData.dims} : pngData.shape.subimageDims,
        .pivot = pngData.shape.pivot,
        .subimagesSpritesCount = pngData.shape.subimagesSpritesCount
    }) {

}

TextureShaped::TextureShaped(TextureShaped&& other) noexcept :
    Texture(std::forward<Texture>(other)),
    m_shape(other.m_shape) {
}

TextureShaped& TextureShaped::operator=(TextureShaped&& other) noexcept {
    Texture::operator=(std::forward<Texture>(other));
    m_shape = other.m_shape;
    return *this;
}

}