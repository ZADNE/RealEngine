#include <RealEngine/TextureFlags.hpp>

namespace RE {

	const TextureFlagsType TextureFlags::RGBA_NU_NEAR_NEAR_EDGE =
		static_cast<TextureFlagsType>(RE::TextureChannels::RGBA) | static_cast<TextureFlagsType>(RE::TextureFormat::NORMALIZED_UNSIGNED) |
		static_cast<TextureFlagsType>(RE::TextureMinFilter::NEAREST) | static_cast<TextureFlagsType>(RE::TextureMagFilter::NEAREST) |
		static_cast<TextureFlagsType>(RE::TextureWrapStyle::CLAMP_TO_EDGE) | (static_cast<TextureFlagsType>(RE::TextureWrapStyle::CLAMP_TO_EDGE) << TEX_WRAP_X_TO_Y_LEFTSHIFT);

	const TextureFlagsType TextureFlags::RGBA_IU_NEAR_NEAR_EDGE =
		static_cast<TextureFlagsType>(RE::TextureChannels::RGBA) | static_cast<TextureFlagsType>(RE::TextureFormat::INTEGRAL_UNSIGNED) |
		static_cast<TextureFlagsType>(RE::TextureMinFilter::NEAREST) | static_cast<TextureFlagsType>(RE::TextureMagFilter::NEAREST) |
		static_cast<TextureFlagsType>(RE::TextureWrapStyle::CLAMP_TO_EDGE) | (static_cast<TextureFlagsType>(RE::TextureWrapStyle::CLAMP_TO_EDGE) << TEX_WRAP_X_TO_Y_LEFTSHIFT);

	TextureFlags::TextureFlags(TextureChannels channels, TextureFormat format,
		TextureMinFilter minFilter, TextureMagFilter magFilter,
		TextureWrapStyle wrapStyleX, TextureWrapStyle wrapStyleY) :
		p_flags(static_cast<TextureFlagsType>(channels) | static_cast<TextureFlagsType>(format) |
			static_cast<TextureFlagsType>(minFilter) | static_cast<TextureFlagsType>(magFilter) |
			static_cast<TextureFlagsType>(wrapStyleX) | (static_cast<TextureFlagsType>(wrapStyleY) << TEX_WRAP_X_TO_Y_LEFTSHIFT)) {

	}

	TextureChannels TextureFlags::getChannels() const {
		return static_cast<TextureChannels>(p_flags & TEX_CHANNELS_BITS);
	}

	TextureFormatType TextureFlags::getFormatType() const {
		return static_cast<TextureFormatType>(p_flags & TEX_FORMAT_TYPE_BITS);
	}

	TextureFormatSign TextureFlags::getFormatSign() const {
		return static_cast<TextureFormatSign>(p_flags & TEX_FORMAT_SIGN_BITS);
	}

	TextureFormat TextureFlags::getFormat() const {
		return static_cast<TextureFormat>(p_flags & TEX_FORMAT_BITS);
	}

	TextureMinFilterMipmapsUsage TextureFlags::getMinFilterMipmapsUsage() const {
		return static_cast<TextureMinFilterMipmapsUsage>(p_flags & TEX_MIN_FILTER_MIPMAPS_USAGE_BITS);
	}

	TextureMinFilterInMipmap TextureFlags::getMinFilterInMipmap() const {
		return static_cast<TextureMinFilterInMipmap>(p_flags & TEX_MIN_FILTER_IN_MIPMAP_BITS);
	}

	TextureMinFilterBetweenMipmaps TextureFlags::getMinFilterBetweenMipmaps() const {
		return static_cast<TextureMinFilterBetweenMipmaps>(p_flags & TEX_MIN_FILTER_BETWEEN_MIPMAPS_BITS);
	}

	TextureMinFilter TextureFlags::getMinFilter() const {
		return static_cast<TextureMinFilter>(p_flags & TEX_MIN_FILTER_BITS);
	}

	TextureMagFilter TextureFlags::getMagFilter() const {
		return static_cast<TextureMagFilter>(p_flags & TEX_MAG_FILTER_BITS);
	}

	TextureWrapStyle TextureFlags::getWrapStyleX() const {
		return static_cast<TextureWrapStyle>(p_flags & TEX_WRAP_X_BITS);
	}

	TextureWrapStyle TextureFlags::getWrapStyleY() const {
		return static_cast<TextureWrapStyle>((p_flags >> TEX_WRAP_Y_TO_X_RIGHTSHIFT) & TEX_WRAP_X_BITS);
	}

	void TextureFlags::setChannels(TextureChannels channels) {
		p_flags = (p_flags & ~TEX_CHANNELS_BITS) | static_cast<TextureFlagsType>(channels);
	}

	void TextureFlags::setFormat(TextureFormat format) {
		p_flags = (p_flags & ~TEX_FORMAT_BITS) | static_cast<TextureFlagsType>(format);
	}

	void TextureFlags::setFormat(TextureFormatType type, TextureFormatSign sign) {
		p_flags = (p_flags & ~TEX_FORMAT_BITS) | static_cast<TextureFlagsType>(type) | static_cast<TextureFlagsType>(sign);
	}

	void TextureFlags::setMagFilter(TextureMagFilter magFilter) {
		p_flags = (p_flags & ~TEX_MAG_FILTER_BITS) | static_cast<TextureFlagsType>(magFilter);
	}

	void TextureFlags::setMinFilter(TextureMinFilter minFilter) {
		p_flags = (p_flags & ~TEX_MIN_FILTER_BITS) | static_cast<TextureFlagsType>(minFilter);
	}

	void TextureFlags::setWrapStyleX(TextureWrapStyle wrapStyle) {
		p_flags = (p_flags & ~TEX_WRAP_X_BITS) | static_cast<TextureFlagsType>(wrapStyle);
	}

	void TextureFlags::setWrapStyleY(TextureWrapStyle wrapStyle) {
		p_flags = (p_flags & ~TEX_WRAP_Y_BITS) | (static_cast<TextureFlagsType>(wrapStyle) << TEX_WRAP_X_TO_Y_LEFTSHIFT);
	}

}