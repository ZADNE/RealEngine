﻿/**
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/textures/TextureShaped.hpp>

namespace re {

TextureShaped::TextureShaped(const PNGLoader::PNGData& pngData)
    : Texture(TextureCreateInfo{.extent = {pngData.dims, 1u}, .texels = pngData.texels})
    , m_shape(TextureShape{
          .subimageDims = pngData.shape.subimageDims == glm::vec2{0.0f, 0.0f}
                              ? glm::vec2{pngData.dims}
                              : pngData.shape.subimageDims,
          .pivot        = pngData.shape.pivot,
          .subimagesSpritesCount = pngData.shape.subimagesSpritesCount
      })
    , m_trueDims(pngData.dims) {
}

TextureShaped::TextureShaped(TextureShaped&& other) noexcept
    : Texture(std::forward<Texture>(other))
    , m_shape(other.m_shape)
    , m_trueDims(other.m_trueDims) {
}

TextureShaped& TextureShaped::operator=(TextureShaped&& other) noexcept {
    Texture::operator=(std::forward<Texture>(other));
    m_shape    = other.m_shape;
    m_trueDims = other.m_trueDims;
    return *this;
}

} // namespace re
