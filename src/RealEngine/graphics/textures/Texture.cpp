/*! 
 *  \author    Dubsky Tomas
 */
#include <RealEngine/graphics/textures/Texture.hpp>

#include <cmath>
#include <iostream>
#include <sstream>

#include <GL/glew.h>
#include <glm/vec4.hpp>
#include <lodepng/lodepng.hpp>

#include <RealEngine/graphics/textures/TextureFlagsToString.hpp>
#include <RealEngine/main/Error.hpp>

namespace RE {

	GLenum toGLenum(TextureChannels channels) {
		static_assert(TEX_CHANNELS_BITS == 0b0000'0000'0000'0011, "TextureFlags - channel bits reaaranged");
		static const GLenum enums[] = {
			GL_RED, GL_RG, GL_RGB, GL_RGBA
		};
		return enums[ft_cast(channels)];
	}

	LodePNGColorType toLodePNGColorType(TextureChannels channels) {
		static_assert(TEX_CHANNELS_BITS == 0b0000'0000'0000'0011, "TextureFlags - channel bits reaaranged");
		static const LodePNGColorType colorTypes[] = {
			LodePNGColorType::LCT_GREY, LodePNGColorType::LCT_GREY_ALPHA, LodePNGColorType::LCT_RGB, LodePNGColorType::LCT_RGBA
		};
		return colorTypes[ft_cast(channels)];
	}

	TextureChannels toTextureChannels(LodePNGColorType colorType) {
		switch (colorType) {
		case LCT_GREY: return TextureChannels::R;
		case LCT_RGB: return TextureChannels::RGB;
		case LCT_GREY_ALPHA: return TextureChannels::RG;
		case LCT_RGBA: return TextureChannels::RGBA;
		default: return TextureChannels::RGBA;
		}
	}

	GLenum textureInternalFormat(TextureChannels channels, TextureFormat type, TextureBitdepthPerChannel bitdepth) {
		static_assert((TEX_CHANNELS_BITS | TEX_FORMAT_BITS) == 0b0000'0000'0000'1111, "TextureFlags - channel or format bits reaaranged");
		static const GLenum internalFormats[3][16] = { {
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
		} };
		return internalFormats[ft_cast(bitdepth) >> TEX_BITDEPTH_TO_VAL_RIGHTSHIFT][ft_cast(channels) | ft_cast(type)];
	}

	int uintlog2(unsigned int arg) {
		if (arg > 0) {
			int log = 0;
			while (arg >>= 1) ++log;
			return log;
		}
		else {
			return 0;
		}
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

	glm::vec4 colorToFloatColor(Color color, TextureFormat type) {
		glm::vec4 borderRGBA = glm::vec4{ color.r, color.g, color.b, color.a };
		switch (type) {
		case TextureFormat::NORMALIZED_UNSIGNED: borderRGBA = borderRGBA / 255.0f; break;
		case TextureFormat::NORMALIZED_SIGNED: borderRGBA = (borderRGBA / 255.0f) * 2.0f - 1.0f; break;
		case TextureFormat::INTEGRAL_UNSIGNED: break;
		case TextureFormat::INTEGRAL_SIGNED: borderRGBA = borderRGBA - 128.0f; break;
		}
		return borderRGBA;
	}

	TextureProxy::TextureProxy(const Texture& texture) :
		m_ID(texture.m_ID) {

	}

	Texture::Texture(Texture&& other) noexcept :
		m_ID(other.m_ID),
		m_flags(other.m_flags),
		m_subimageDims(other.m_subimageDims),
		m_pivot(other.m_pivot),
		m_subimagesSpritesCount(other.m_subimagesSpritesCount),
		m_trueDims(other.m_trueDims),
		m_borderColor(other.m_borderColor) {
		other.m_ID = 0u;
	}

	Texture& Texture::operator=(Texture&& other) noexcept {
		std::swap(m_ID, other.m_ID);
#ifdef _DEBUG
		//Swap flags and dims to correctly construct identificationString() in destructor
		std::swap(m_flags, other.m_flags);
		std::swap(m_trueDims, other.m_trueDims);
#else
		//identificationString() is only in debug destructor
		m_flags = other.m_flags;
		m_trueDims = other.m_trueDims;
#endif // _DEBUG
		m_subimageDims = other.m_subimageDims;
		m_pivot = other.m_pivot;
		m_subimagesSpritesCount = other.m_subimagesSpritesCount;
		m_borderColor = other.m_borderColor;
		return *this;
	}

	Texture::Texture(const std::string& filePathPNG) {
		//Prepare variables
		lodepng::State state{};
		state.decoder.remember_unknown_chunks = 1;
		std::vector<unsigned char> png, pixels;
		glm::uvec2 dims;
		unsigned int code;

		//Decode PNG
		if ((code = lodepng::load_file(png, filePathPNG)) || (code = lodepng::decode(pixels, dims.x, dims.y, state, png))) {
			//Loading or decoding failed
			error(filePathPNG + ": " + lodepng_error_text(code));
			return;//Texture will be empty
		}

		//Load parameters
		TextureParameters params = DEFAULT_PARAMETERS;
		for (size_t unknownChunkPos = 0; unknownChunkPos < 3; unknownChunkPos++) {
			const auto& unknownData = state.info_png.unknown_chunks_data[0];
			const auto& unknownDataEnd = &unknownData[state.info_png.unknown_chunks_size[0]];
			for (auto chunk = unknownData; chunk != unknownDataEnd; chunk = lodepng_chunk_next(chunk, unknownDataEnd)) {
				char type[5];
				lodepng_chunk_type(type, chunk);
				if (std::string{ "reAl" } == type) {
					try {
						params = TextureParameters{ lodepng_chunk_data(chunk), lodepng_chunk_length(chunk) };
						goto paramsLoaded;
					}
					catch (const char* e) {
						error(std::string{ "RTI decode fail: " } + e);
					}
				}
			}
		}

	paramsLoaded:
		//Initialize texture
		init(Raster{ dims, toTextureChannels(state.info_raw.colortype), pixels }, params);
	}

	Texture::Texture(const Raster& raster, const TextureParameters& params/* = DEFAULT_PARAMETERS*/) {
		init(raster, params);
	}

	Texture::~Texture() {
		if (m_ID != 0) {
			glDeleteTextures(1, &m_ID);
#if defined(_DEBUG) && defined(RE_LOG_TEXTURE_DESTROYED)
			std::cout << "Destroyed " << identificationString() << '\n';
#endif // defined(_DEBUG) && defined(RE_LOG_TEXTURE_DESTROYED)
		}
	}

	TextureParameters Texture::getParameters() const {
		return TextureParameters{ TextureGeometry{m_subimageDims, m_pivot, m_subimagesSpritesCount}, m_flags, m_borderColor };
	}

	void Texture::setMinFilter(TextureMinFilter minFilter) {
		m_flags.setMinFilter(minFilter);
		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, minFilterToGLEnum(minFilter));
	}

	void Texture::setMagFilter(TextureMagFilter magFilter) {
		m_flags.setMagFilter(magFilter);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, (magFilter == TextureMagFilter::LINEAR) ? GL_LINEAR : GL_NEAREST);
	}

	void Texture::setWrapStyleX(TextureWrapStyle wrapStyleX) {
		m_flags.setWrapStyleX(wrapStyleX);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, wrapStyleToGLEnum(wrapStyleX));
	}

	void Texture::setWrapStyleY(TextureWrapStyle wrapStyleY) {
		m_flags.setWrapStyleY(wrapStyleY);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, wrapStyleToGLEnum(wrapStyleY));
	}

	void Texture::setBorderColor(RE::Color col) {
		m_borderColor = col;
		glm::vec4 borderRGBA = colorToFloatColor(col, getFormat());
		glTextureParameterfv(m_ID, GL_TEXTURE_BORDER_COLOR, &borderRGBA.r);
	}

	void Texture::setBorderColor(const glm::vec4& col) {
		m_borderColor = RE::Color{ col * 255.0f };
		glTextureParameterfv(m_ID, GL_TEXTURE_BORDER_COLOR, &col.r);
	}

	void Texture::bindImage(ImageUnit unit, GLint level, ImageAccess access) const {
		glBindImageTexture(unit.m_unit, m_ID, level, GL_FALSE, 0, static_cast<GLenum>(access), textureInternalFormat(getChannels(), getFormat(), getBitdepthPerChannel()));
	}

	void Texture::setTexelsWithinImage(GLint level, const glm::ivec2& offset, const glm::ivec2& size, const void* raster) const {
		glTextureSubImage2D(m_ID, level, offset.x, offset.y,
			size.x, size.y, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, raster);
	}

	void Texture::copyTexelsBetweenImages(GLint srcLevel, const glm::ivec2& srcPos, const Texture& destination, GLint dstLevel, const glm::ivec2& dstPos, const glm::ivec2& size) const {
		glCopyImageSubData(m_ID, GL_TEXTURE_2D, srcLevel, srcPos.x, srcPos.y, 0,
			destination.m_ID, GL_TEXTURE_2D, dstLevel, dstPos.x, dstPos.y, 0, size.x, size.y, 1);
	}

	void Texture::clear(const glm::vec4& color) const {
		glClearTexImage(m_ID, 0, GL_RGBA, GL_FLOAT, &color);
	}

	void Texture::clear(const glm::ivec4& color) const {
		glClearTexImage(m_ID, 0, GL_RGBA_INTEGER, GL_INT, &color);
	}

	void Texture::clear(const glm::uvec4& color) const {
		glClearTexImage(m_ID, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT, &color);
	}

	void Texture::clear(RE::Color color) const {
		glClearTexImage(m_ID, 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, &color);
	}

	bool Texture::saveToFile(const std::string& filePathPNG) {
		return saveToFile(filePathPNG, getParameters());
	}

	bool Texture::saveToFile(const std::string& filePathPNG, const TextureParameters& params) {
		auto channels = params.getChannels();
		//Download pixels
		std::vector<unsigned char> pixels;
		pixels.resize(Raster::requiredMemory(m_trueDims, channels));
		glGetTextureImage(m_ID, 0, toGLenum(channels), GL_UNSIGNED_BYTE, static_cast<GLsizei>(pixels.size()), pixels.data());

		//Insert rti chunk
		lodepng::State state{};
		auto rti = params.convertToRTI();
		if (lodepng_chunk_create(&state.info_png.unknown_chunks_data[0], &state.info_png.unknown_chunks_size[0], static_cast<unsigned int>(rti.size()), "reAl", rti.data())) {
			//Chunk creation failed
			return false;
		}

		//Encode and save PNG
		unsigned int code;
		state.info_png.color.colortype = toLodePNGColorType(channels);
		state.encoder.auto_convert = 0;
		std::vector<unsigned char> png;
		if ((code = lodepng::encode(png, pixels, m_trueDims.x, m_trueDims.y, state)) || (code = lodepng::save_file(png, filePathPNG))) {
			//Encoding or saving failed
			return false;
		}

		return true;
	}

	std::string Texture::identificationString() const {
		std::stringstream stream;
		stream << "texture (ID: " << m_ID << " - " << m_trueDims.x << "x" << m_trueDims.y << "x" << to_string(getChannels()) << ")";
		return stream.str();
	}

	void Texture::init(const Raster& raster, const TextureParameters& params) {
		if (params.isGeometryDefinedByImage()) {//Geometry defined by image
			m_subimageDims = raster.getDims();
			m_pivot = glm::vec2{ 0.0f, 0.0f };
			m_subimagesSpritesCount = glm::vec2{ 1.0f, 1.0f };
		}
		else {//Geometry defined by parameters
			m_subimageDims = params.getSubimageDims();
			m_pivot = params.getPivot();
			m_subimagesSpritesCount = params.getSubimagesSpritesCount();
		}
		m_flags = TextureFlags{ params };
		m_trueDims = raster.getDims();
		m_borderColor = params.getBorderColor();

		//Create texture
		glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

		//Create storage for the texture
		GLsizei levels = getMinFilterMipmapsUsage() == TextureMinFilterMipmapsUsage::YES ? uintlog2(std::max(m_trueDims.x, m_trueDims.y)) : 1;
		auto internalFormat = textureInternalFormat(getChannels(), getFormat(), getBitdepthPerChannel());
		glTextureStorage2D(m_ID, levels, internalFormat, m_trueDims.x, m_trueDims.y);

		//Upload pixels for the texture (if specified)
		if (raster.getTexels().size() > 0u) {
			glTextureSubImage2D(m_ID, 0, 0, 0, m_trueDims.x, m_trueDims.y, toGLenum(raster.getChannels()), GL_UNSIGNED_BYTE, raster.getTexels().data());
		}

		//Set parameters of the texture
		setMinFilter(getMinFilter());
		setMagFilter(getMagFilter());
		setWrapStyleX(getWrapStyleX());
		setWrapStyleY(getWrapStyleY());
		setBorderColor(getBorderColor());

		//Generate mipmaps for the texture
		if (levels > 1) {
			glGenerateTextureMipmap(m_ID);
		}

#if defined(_DEBUG) && defined(RE_LOG_TEXTURE_CREATED)
		std::cout << "Created " << identificationString() << '\n';
#endif // defined(_DEBUG) && defined(RE_LOG_TEXTURE_CREATED)
	}

}