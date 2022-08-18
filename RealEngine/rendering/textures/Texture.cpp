/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/textures/Texture.hpp>

#include <lodepng/lodepng.hpp>

#include <RealEngine/rendering/textures/TextureFlagsToString.hpp>
#include <RealEngine/utility/error.hpp>

namespace RE {

ITexture* Texture::s_impl = nullptr;
ITexture* TextureProxy::s_impl = nullptr;

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

glm::vec4 colorToFloatColor(Color color, TextureFormat type) {
	glm::vec4 borderRGBA = glm::vec4{color.r, color.g, color.b, color.a};
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

void TextureProxy::bind() const {
	s_impl->bind(*this);
}

void TextureProxy::bind(TextureUnit unit) const {
	s_impl->bind(*this, unit);
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
			if (std::string{"reAl"} == type) {
				try {
					params = TextureParameters{lodepng_chunk_data(chunk), lodepng_chunk_length(chunk)};
					goto paramsLoaded;
				}
				catch (const char* e) {
					error(std::string{"RTI decode fail: "} + e);
				}
			}
		}
	}

paramsLoaded:
	//Initialize texture
	init(Raster{dims, toTextureChannels(state.info_raw.colortype), pixels}, params);
}

Texture::Texture(const Raster& raster, const TextureParameters& params/* = DEFAULT_PARAMETERS*/) {
	init(raster, params);
}

Texture::~Texture() {
	s_impl->destruct(*this);
}

TextureParameters Texture::getParameters() const {
	return TextureParameters{TextureGeometry{m_subimageDims, m_pivot, m_subimagesSpritesCount}, m_flags, m_borderColor};
}

void Texture::setMinFilter(TextureMinFilter minFilter) {
	m_flags.setMinFilter(minFilter);
	s_impl->setMinFilter(*this, minFilter);
}

void Texture::setMagFilter(TextureMagFilter magFilter) {
	m_flags.setMagFilter(magFilter);
	s_impl->setMagFilter(*this, magFilter);
}

void Texture::setWrapStyleX(TextureWrapStyle wrapStyleX) {
	m_flags.setWrapStyleX(wrapStyleX);
	s_impl->setWrapStyleX(*this, wrapStyleX);
}

void Texture::setWrapStyleY(TextureWrapStyle wrapStyleY) {
	m_flags.setWrapStyleY(wrapStyleY);
	s_impl->setWrapStyleY(*this, wrapStyleY);
}

void Texture::setBorderColor(Color col) {
	m_borderColor = col;
	glm::vec4 borderRGBA = colorToFloatColor(col, getFormat());
	s_impl->setBorderColor(*this, m_borderColor);
}

void Texture::setBorderColor(const glm::vec4& col) {
	m_borderColor = Color{col * 255.0f};
	s_impl->setBorderColor(*this, col);
}

void Texture::bind() const {
	s_impl->bind(*this);
}

void Texture::bind(TextureUnit unit) const {
	s_impl->bind(*this, unit);
}

void Texture::bindImage(ImageUnit unit, int level, ImageAccess access) const {
	s_impl->bindImage(*this, unit, level, access);
}

void Texture::setTexels(int level, const glm::ivec2& offset, const glm::ivec2& size, const void* raster) const {
	s_impl->setTexels(*this, level, offset, size, raster);
}

void Texture::copyTexels(int srcLevel, const glm::ivec2& srcPos, const Texture& destination, int dstLevel, const glm::ivec2& dstPos, const glm::ivec2& size) const {
	s_impl->copyTexels(*this, srcLevel, srcPos, destination, dstLevel, dstPos, size);
}

void Texture::clear(const glm::vec4& color) const {
	s_impl->clear(*this, 0, color);
}

void Texture::clear(const glm::ivec4& color) const {
	s_impl->clear(*this, 0, color);
}

void Texture::clear(const glm::uvec4& color) const {
	s_impl->clear(*this, 0, color);
}

void Texture::clear(Color color) const {
	s_impl->clear(*this, 0, color);
}

bool Texture::saveToFile(const std::string& filePathPNG) {
	return saveToFile(filePathPNG, getParameters());
}

bool Texture::saveToFile(const std::string& filePathPNG, const TextureParameters& params) {
	auto channels = params.getChannels();
	//Download pixels
	std::vector<unsigned char> pixels;
	pixels.resize(Raster::minimumRequiredMemory(m_trueDims, channels));
	s_impl->getTexels(*this, 0, static_cast<int>(pixels.size()), pixels.data());

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

void Texture::init(const Raster& raster, const TextureParameters& params) {
	if (params.isGeometryDefinedByImage()) {//Geometry defined by image
		m_subimageDims = raster.getDims();
		m_pivot = glm::vec2{0.0f, 0.0f};
		m_subimagesSpritesCount = glm::vec2{1.0f, 1.0f};
	} else {//Geometry defined by parameters
		m_subimageDims = params.getSubimageDims();
		m_pivot = params.getPivot();
		m_subimagesSpritesCount = params.getSubimagesSpritesCount();
	}
	m_flags = TextureFlags{params};
	m_trueDims = raster.getDims();
	m_borderColor = params.getBorderColor();

	s_impl->construct(*this, raster);

	//Set parameters of the texture
	setMinFilter(getMinFilter());
	setMagFilter(getMagFilter());
	setWrapStyleX(getWrapStyleX());
	setWrapStyleY(getWrapStyleY());
	setBorderColor(getBorderColor());
}

}