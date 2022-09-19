/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/VK13Texture.hpp>

namespace RE {

TextureID VK13Texture::construct(TextureFlags flags, const Raster& raster) const {
    return TextureID{0};
}

void VK13Texture::destruct(TextureID& te) const {

}

void VK13Texture::setMinFilter(TextureID& te, TextureMinFilter minFilter) const {

}

void VK13Texture::setMagFilter(TextureID& te, TextureMagFilter magFilter) const {

}

void VK13Texture::setWrapStyleX(TextureID& te, TextureWrapStyle wrapStyleX) const {

}

void VK13Texture::setWrapStyleY(TextureID& te, TextureWrapStyle wrapStyleY) const {

}

void VK13Texture::setBorderColor(TextureID& te, const glm::vec4& col) const {

}

void VK13Texture::bind(const TextureID& te) const {

}

void VK13Texture::bind(const TextureID& te, TextureUnit unit) const {

}

void VK13Texture::bindImage(const TextureID& te, ImageUnit unit, int level, ImageAccess access, TextureFlags flags) const {

}

void VK13Texture::setTexels(const TextureID& te, int level, const glm::ivec2& offset, const glm::ivec2& size, TextureFlags flags, const void* raster) const {

}

void VK13Texture::copyTexels(const TextureID& te, int srcLevel, const glm::ivec2& srcPos, const TextureID& destination, int dstLevel, const glm::ivec2& dstPos, const glm::ivec2& size) const {

}

void VK13Texture::getTexels(const TextureID& te, int level, const glm::ivec2& offset, const glm::ivec2& size, TextureFlags flags, size_t bufSize, void* pixels) const {

}

void VK13Texture::clear(const TextureID& te, int level, const glm::vec4& color) const {

}

void VK13Texture::clear(const TextureID& te, int level, const glm::ivec4& color) const {

}

void VK13Texture::clear(const TextureID& te, int level, const glm::uvec4& color) const {

}

void VK13Texture::clear(const TextureID& te, int level, const Color& color) const {

}

}