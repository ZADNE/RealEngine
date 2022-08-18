/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/GL46_Texture.hpp>

#include <GL/glew.h>

#include <RealEngine/rendering/textures/Texture.hpp>
#include <RealEngine/utility/math.hpp>
#include <RealEngine/utility/error.hpp>

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
		GL_R8,		GL_RG8,			GL_RGB8,		GL_RGBA8,
		GL_R8_SNORM,GL_RG8_SNORM,	GL_RGB8_SNORM,	GL_RGBA8_SNORM,
		GL_R8UI,	GL_RG8UI,		GL_RGB8UI,		GL_RGBA8UI,
		GL_R8I,		GL_RG8I,		GL_RGB8I,		GL_RGBA8I
	},{
		GL_R16,		GL_RG16,		GL_RGB16,		GL_RGBA16,
		GL_R16_SNORM,GL_RG16_SNORM,	GL_RGB16_SNORM,	GL_RGBA16_SNORM,
		GL_R16UI,	GL_RG16UI,		GL_RGB16UI,		GL_RGBA16UI,
		GL_R16I,	GL_RG16I,		GL_RGB16I,		GL_RGBA16I
	},{
		GL_R32F,	GL_RG32F,		GL_RGB32F,		GL_RGBA32F,
		GL_R32F,	GL_RG32F,		GL_RGB32F,		GL_RGBA32F,
		GL_R32UI,	GL_RG32UI,		GL_RGB32UI,		GL_RGBA32UI,
		GL_R32I,	GL_RG32I,		GL_RGB32I,		GL_RGBA32I
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

void GL46_Texture::construct(Texture& te, const Raster& raster) const {
	//Create texture
	glCreateTextures(GL_TEXTURE_2D, 1, &te.m_ID);

	//Create storage for the texture
	GLsizei levels = te.getMinFilterMipmapsUsage() == TextureMinFilterMipmapsUsage::YES ? uintlog2(std::max(te.m_trueDims.x, te.m_trueDims.y)) + 1 : 1;
	auto internalFormat = convert(te.getChannels(), te.getFormat(), te.getBitdepthPerChannel());
	glTextureStorage2D(te.m_ID, levels, internalFormat, te.m_trueDims.x, te.m_trueDims.y);

	//Upload pixels for the texture (if specified)
	if (raster.getTexels().size() > 0u) {
		glTextureSubImage2D(te.m_ID, 0, 0, 0, te.m_trueDims.x, te.m_trueDims.y, convert(raster.getChannels()), GL_UNSIGNED_BYTE, raster.getTexels().data());
	}

	//Generate mipmaps for the texture
	if (levels > 1) {
		glGenerateTextureMipmap(te.m_ID);
	}
}

void GL46_Texture::destruct(Texture& te) const {
	glDeleteTextures(1, &te.m_ID);
}

void GL46_Texture::setMinFilter(Texture& te, TextureMinFilter minFilter) const {
	glTextureParameteri(te.m_ID, GL_TEXTURE_MIN_FILTER, minFilterToGLEnum(minFilter));
}

void GL46_Texture::setMagFilter(Texture& te, TextureMagFilter magFilter) const {
	glTextureParameteri(te.m_ID, GL_TEXTURE_MAG_FILTER, (magFilter == TextureMagFilter::LINEAR) ? GL_LINEAR : GL_NEAREST);
}

void GL46_Texture::setWrapStyleX(Texture& te, TextureWrapStyle wrapStyleX) const {
	glTextureParameteri(te.m_ID, GL_TEXTURE_WRAP_S, wrapStyleToGLEnum(wrapStyleX));
}

void GL46_Texture::setWrapStyleY(Texture& te, TextureWrapStyle wrapStyleY) const {
	glTextureParameteri(te.m_ID, GL_TEXTURE_WRAP_T, wrapStyleToGLEnum(wrapStyleY));
}

void GL46_Texture::setBorderColor(Texture& te, const glm::vec4& col) const {
	glTextureParameterfv(te.m_ID, GL_TEXTURE_BORDER_COLOR, &col.r);
}

void GL46_Texture::bind(const Texture& te) const {
	glBindTexture(GL_TEXTURE_2D, te.m_ID);
}

void GL46_Texture::bind(const TextureProxy& te) const {
	glBindTexture(GL_TEXTURE_2D, te.m_ID);
}

void GL46_Texture::bind(const Texture& te, TextureUnit unit) const {
	glBindTextureUnit(unit.m_unit, te.m_ID);
}

void GL46_Texture::bind(const TextureProxy& te, TextureUnit unit) const {
	glBindTextureUnit(unit.m_unit, te.m_ID);
}

void GL46_Texture::bindImage(const Texture& te, ImageUnit unit, int level, ImageAccess access) const {
	glBindImageTexture(unit.m_unit, te.m_ID, level, GL_FALSE, 0, convert(access), convert(te.getChannels(), te.getFormat(), te.getBitdepthPerChannel()));
}

void GL46_Texture::setTexels(const Texture& te, int level, const glm::ivec2& offset, const glm::ivec2& size, const void* raster) const {
	glTextureSubImage2D(te.m_ID, level, offset.x, offset.y,
		size.x, size.y, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, raster);
}

void GL46_Texture::copyTexels(const Texture& te, int srcLevel, const glm::ivec2& srcPos, const Texture& destination, int dstLevel, const glm::ivec2& dstPos, const glm::ivec2& size) const {
	glCopyImageSubData(te.m_ID, GL_TEXTURE_2D, srcLevel, srcPos.x, srcPos.y, 0,
		destination.m_ID, GL_TEXTURE_2D, dstLevel, dstPos.x, dstPos.y, 0, size.x, size.y, 1);
}

void GL46_Texture::getTexels(const Texture& te, int level, int bufSize, void* pixels) const {
	glGetTextureImage(te.m_ID, 0, convert(te.getChannels()), GL_UNSIGNED_BYTE, bufSize, pixels);
}

void GL46_Texture::clear(const Texture& te, int level, const glm::vec4& color) const {
	glClearTexImage(te.m_ID, level, GL_RGBA, GL_FLOAT, &color);
}

void GL46_Texture::clear(const Texture& te, int level, const glm::ivec4& color) const {
	glClearTexImage(te.m_ID, level, GL_RGBA_INTEGER, GL_INT, &color);
}

void GL46_Texture::clear(const Texture& te, int level, const glm::uvec4& color) const {
	glClearTexImage(te.m_ID, level, GL_RGBA_INTEGER, GL_UNSIGNED_INT, &color);
}

void GL46_Texture::clear(const Texture& te, int level, const Color& color) const {
	glClearTexImage(te.m_ID, level, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, &color);
}

}