/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/GL46Texture.hpp>

#include <GL/glew.h>

#include <glm/gtx/texture.hpp>

#include <RealEngine/rendering/textures/Texture.hpp>
#include <RealEngine/utility/Error.hpp>

namespace RE {

GLenum convert(TextureChannels channels) {
    static_assert(TEX_CHANNELS_BITS == 0b0000'0000'0000'0011, "TextureFlags - channel bits reaaranged");
    static const GLenum enums[] = {
        GL_RED, GL_RG, GL_RGB, GL_RGBA
    };
    return enums[ft_cast(channels)];
}

GLenum convert(ImageAccess access) {
    switch (access) {
    case ImageAccess::READ_ONLY: return GL_READ_ONLY;
    case ImageAccess::WRITE_ONLY: return GL_WRITE_ONLY;
    case ImageAccess::READ_WRITE: return GL_READ_WRITE;
    default: fatalError("Bad enum value of ImageAccess!");
    }
}

GLenum convert(TextureChannels channels, TextureFormat type, TextureBitdepthPerChannel bitdepth) {
    static_assert((TEX_CHANNELS_BITS | TEX_FORMAT_BITS) == 0b0000'0000'0000'1111, "TextureFlags - channel or format bits reaaranged");
    static const GLenum internalFormats[3][16] = {{
        GL_R8,          GL_RG8,         GL_RGB8,        GL_RGBA8,
        GL_R8_SNORM,    GL_RG8_SNORM,   GL_RGB8_SNORM,  GL_RGBA8_SNORM,
        GL_R8UI,        GL_RG8UI,       GL_RGB8UI,      GL_RGBA8UI,
        GL_R8I,         GL_RG8I,        GL_RGB8I,       GL_RGBA8I
    },{
        GL_R16,         GL_RG16,        GL_RGB16,       GL_RGBA16,
        GL_R16_SNORM,   GL_RG16_SNORM,  GL_RGB16_SNORM, GL_RGBA16_SNORM,
        GL_R16UI,       GL_RG16UI,      GL_RGB16UI,     GL_RGBA16UI,
        GL_R16I,        GL_RG16I,       GL_RGB16I,      GL_RGBA16I
    },{
        GL_R32F,        GL_RG32F,       GL_RGB32F,      GL_RGBA32F,
        GL_R32F,        GL_RG32F,       GL_RGB32F,      GL_RGBA32F,
        GL_R32UI,       GL_RG32UI,      GL_RGB32UI,     GL_RGBA32UI,
        GL_R32I,        GL_RG32I,       GL_RGB32I,      GL_RGBA32I
    }};
    return internalFormats[ft_cast(bitdepth) >> TEX_BITDEPTH_TO_VAL_RIGHTSHIFT][ft_cast(channels) | ft_cast(type)];
}

int wrapStyleToGLEnum(TextureWrapStyle style) {
    switch (style) {
    case RE::TextureWrapStyle::CLAMP_TO_EDGE: return GL_CLAMP_TO_EDGE;
    case RE::TextureWrapStyle::CLAMP_TO_BORDER: return GL_CLAMP_TO_BORDER;
    case RE::TextureWrapStyle::REPEAT_NORMALLY: return GL_REPEAT;
    case RE::TextureWrapStyle::REPEAT_MIRRORED: return GL_MIRRORED_REPEAT;
    default:
        return GL_CLAMP_TO_EDGE;
    }
}

int minFilterToGLEnum(TextureMinFilter filter) {
    switch (filter) {
    case RE::TextureMinFilter::NEAREST_NO_MIPMAPS: return GL_NEAREST;
    case RE::TextureMinFilter::LINEAR_NO_MIPMAPS: return GL_LINEAR;
    case RE::TextureMinFilter::NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
    case RE::TextureMinFilter::NEAREST_MIPMAP_LINEAR: return GL_NEAREST_MIPMAP_LINEAR;
    case RE::TextureMinFilter::LINEAR_MIPMAP_NEAREST: return GL_LINEAR_MIPMAP_NEAREST;
    case RE::TextureMinFilter::LINEAR_MIPMAP_LINEAR: return GL_LINEAR_MIPMAP_LINEAR;
    default:
        return GL_NEAREST;
    }
}

TextureID GL46Texture::construct(TextureFlags flags, const Raster& raster) const {
    //Create texture
    unsigned int id;
    glCreateTextures(GL_TEXTURE_2D, 1, &id);

    //Create storage for the texture
    auto dims = raster.getDims();
    GLsizei levels = flags.getMinFilterMipmapsUsage() == TextureMinFilterMipmapsUsage::YES ? glm::levels(dims) : 1;
    auto internalFormat = convert(flags.getChannels(), flags.getFormat(), flags.getBitdepthPerChannel());
    glTextureStorage2D(id, levels, internalFormat, dims.x, dims.y);

    //Upload pixels for the texture (if specified)
    if (raster.getTexels().size() > 0u) {
        glTextureSubImage2D(id, 0, 0, 0, dims.x, dims.y, convert(raster.getChannels()), GL_UNSIGNED_BYTE, raster.getTexels().data());
    }

    //Generate mipmaps for the texture
    if (levels > 1) {
        glGenerateTextureMipmap(id);
    }

    return TextureID{id};
}

void GL46Texture::destruct(TextureID& te) const {
    glDeleteTextures(1, &te.m_id);
}

void GL46Texture::setMinFilter(TextureID& te, TextureMinFilter minFilter) const {
    glTextureParameteri(te.m_id, GL_TEXTURE_MIN_FILTER, minFilterToGLEnum(minFilter));
}

void GL46Texture::setMagFilter(TextureID& te, TextureMagFilter magFilter) const {
    glTextureParameteri(te.m_id, GL_TEXTURE_MAG_FILTER, (magFilter == TextureMagFilter::LINEAR) ? GL_LINEAR : GL_NEAREST);
}

void GL46Texture::setWrapStyleX(TextureID& te, TextureWrapStyle wrapStyleX) const {
    glTextureParameteri(te.m_id, GL_TEXTURE_WRAP_S, wrapStyleToGLEnum(wrapStyleX));
}

void GL46Texture::setWrapStyleY(TextureID& te, TextureWrapStyle wrapStyleY) const {
    glTextureParameteri(te.m_id, GL_TEXTURE_WRAP_T, wrapStyleToGLEnum(wrapStyleY));
}

void GL46Texture::setBorderColor(TextureID& te, const glm::vec4& col) const {
    glTextureParameterfv(te.m_id, GL_TEXTURE_BORDER_COLOR, &col.r);
}

void GL46Texture::bind(const TextureID& te) const {
    glBindTexture(GL_TEXTURE_2D, te.m_id);
}

void GL46Texture::bind(const TextureID& te, TextureUnit unit) const {
    glBindTextureUnit(unit.m_unit, te.m_id);
}

void GL46Texture::bindImage(const TextureID& te, ImageUnit unit, int level, ImageAccess access, TextureFlags flags) const {
    glBindImageTexture(unit.m_unit, te.m_id, level, GL_FALSE, 0, convert(access), convert(flags.getChannels(), flags.getFormat(), flags.getBitdepthPerChannel()));
}

void GL46Texture::setTexels(const TextureID& te, int level, const glm::ivec2& offset, const glm::ivec2& size, TextureFlags flags, const void* raster) const {
    auto integer = flags.getFormatType() == TextureFormatType::INTEGRAL;
    glTextureSubImage2D(te.m_id, level, offset.x, offset.y,
        size.x, size.y, integer ? GL_RGBA_INTEGER : GL_RGBA, GL_UNSIGNED_BYTE, raster);
}

void GL46Texture::copyTexels(const TextureID& te, int srcLevel, const glm::ivec2& srcPos, const TextureID& destination, int dstLevel, const glm::ivec2& dstPos, const glm::ivec2& size) const {
    glCopyImageSubData(te.m_id, GL_TEXTURE_2D, srcLevel, srcPos.x, srcPos.y, 0,
        destination.m_id, GL_TEXTURE_2D, dstLevel, dstPos.x, dstPos.y, 0, size.x, size.y, 1);
}

void GL46Texture::getTexels(const TextureID& te, int level, const glm::ivec2& offset, const glm::ivec2& size, TextureFlags flags, size_t bufSize, void* pixels) const {
    auto integer = flags.getFormatType() == TextureFormatType::INTEGRAL;
    glGetTextureSubImage(
        te.m_id, 0,
        offset.x, offset.y, 0,
        size.x, size.y, 1,
        integer ? GL_RGBA_INTEGER : GL_RGBA, GL_UNSIGNED_BYTE, static_cast<GLsizei>(bufSize), pixels);
}

void GL46Texture::clear(const TextureID& te, int level, const glm::vec4& color) const {
    glClearTexImage(te.m_id, level, GL_RGBA, GL_FLOAT, &color);
}

void GL46Texture::clear(const TextureID& te, int level, const glm::ivec4& color) const {
    glClearTexImage(te.m_id, level, GL_RGBA_INTEGER, GL_INT, &color);
}

void GL46Texture::clear(const TextureID& te, int level, const glm::uvec4& color) const {
    glClearTexImage(te.m_id, level, GL_RGBA_INTEGER, GL_UNSIGNED_INT, &color);
}

void GL46Texture::clear(const TextureID& te, int level, const Color& color) const {
    glClearTexImage(te.m_id, level, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, &color);
}

}