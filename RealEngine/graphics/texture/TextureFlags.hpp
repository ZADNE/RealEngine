#pragma once
/**
 * @file TextureFlags.hpp
 */
#include <cstdint>

namespace RE {

using TextureFlagsType = uint16_t;

const TextureFlagsType TEX_CHANNELS_BITS = 0b0000'0000'0000'0011;
const TextureFlagsType TEX_FORMAT_SIGN_BITS = 0b0000'0000'0000'0100;
const TextureFlagsType TEX_FORMAT_TYPE_BITS = 0b0000'0000'0000'1000;
const TextureFlagsType TEX_FORMAT_BITS = TEX_FORMAT_SIGN_BITS | TEX_FORMAT_TYPE_BITS;
const TextureFlagsType TEX_MIN_FILTER_MIPMAPS_USAGE_BITS = 0b0000'0000'0001'0000;
const TextureFlagsType TEX_MIN_FILTER_IN_MIPMAP_BITS = 0b0000'0000'0010'0000;
const TextureFlagsType TEX_MIN_FILTER_BETWEEN_MIPMAPS_BITS = 0b0000'0000'0100'0000;
const TextureFlagsType TEX_MIN_FILTER_BITS = TEX_MIN_FILTER_MIPMAPS_USAGE_BITS | TEX_MIN_FILTER_IN_MIPMAP_BITS | TEX_MIN_FILTER_BETWEEN_MIPMAPS_BITS;
const TextureFlagsType TEX_MAG_FILTER_BITS = 0b0000'0000'1000'0000;
const TextureFlagsType TEX_WRAP_STYLE_X_BITS = 0b0000'0001'0000'0000;
const TextureFlagsType TEX_REPEAT_STYLE_X_BITS = 0b0000'0010'0000'0000;
const TextureFlagsType TEX_CLAMP_STYLE_X_BITS = TEX_REPEAT_STYLE_X_BITS;
const TextureFlagsType TEX_WRAP_X_BITS = TEX_WRAP_STYLE_X_BITS | TEX_REPEAT_STYLE_X_BITS;
const TextureFlagsType TEX_WRAP_STYLE_Y_BITS = 0b0000'0100'0000'0000;
const TextureFlagsType TEX_REPEAT_STYLE_Y_BITS = 0b0000'1000'0000'0000;
const TextureFlagsType TEX_CLAMP_STYLE_Y_BITS = TEX_REPEAT_STYLE_Y_BITS;
const TextureFlagsType TEX_WRAP_Y_BITS = TEX_WRAP_STYLE_Y_BITS | TEX_REPEAT_STYLE_Y_BITS;
const TextureFlagsType TEX_WRAP_Y_TO_X_RIGHTSHIFT = 2;
const TextureFlagsType TEX_WRAP_X_TO_Y_LEFTSHIFT = 2;

static_assert(
	TEX_WRAP_X_BITS == (TEX_WRAP_Y_BITS >> TEX_WRAP_Y_TO_X_RIGHTSHIFT)
	&& TEX_WRAP_Y_BITS == (TEX_WRAP_X_BITS << TEX_WRAP_X_TO_Y_LEFTSHIFT), "TextureFlags - wrap x or y rearranged");

static_assert(TEX_CHANNELS_BITS == 0b0000'0000'0000'0011, "TextureFlags - channel bits rearranged");

/**
 * @brief Texture can have 1-4 channels
*/
enum class TextureChannels : TextureFlagsType {
	R = TEX_CHANNELS_BITS - 3,	/**< Red channel only*/
	RG = TEX_CHANNELS_BITS - 2,	/**< Red and green channels*/
	RGB = TEX_CHANNELS_BITS - 1,/**< Red, green and blue channels*/
	RGBA = TEX_CHANNELS_BITS - 0/**< Red, green, blue and alpha channels*/
};

/**
 * @brief Sign of format specifies whether the values will be signed or unsigned
*/
enum class TextureFormatSign : TextureFlagsType {
	UNSIGNED = 0,					/**< Positive values only */
	SIGNED = TEX_FORMAT_SIGN_BITS	/**< Positive and negative values */
};

/**
 * @brief Type of format specifies whether the values are normalized
*/
enum class TextureFormatType : TextureFlagsType {
	NORMALIZED = 0,					/**< Values [0, 1] or [-1, 1], depending on sign */
	INTEGRAL = TEX_FORMAT_TYPE_BITS	/**< Values [0, 255] or [-128, 127], depending on sign */
};

/**
 * @brief Format of texture specifies the internal format that will be used when drawing the texture.
 * Ii can be normalized/integral and unsigned/signed.
*/
enum class TextureFormat : TextureFlagsType {
	NORMALIZED_UNSIGNED = 0,										/**< Values in range [0, 1]*/
	NORMALIZED_SIGNED = TEX_FORMAT_SIGN_BITS,						/**< Values in range [-1, 1]*/
	INTEGRAL_UNSIGNED = TEX_FORMAT_TYPE_BITS,						/**< Values in range [0, 255]*/
	INTEGRAL_SIGNED = TEX_FORMAT_TYPE_BITS | TEX_FORMAT_SIGN_BITS	/**< Values in range [-128, 127]*/
};

/**
 * @brief Minifying with mipmaps is nicer but slower.
*/
enum class TextureMinFilterMipmapsUsage : TextureFlagsType {
	NO,										/**< Do not use mipmaps - fast */
	YES = TEX_MIN_FILTER_MIPMAPS_USAGE_BITS /**< Do use mipmaps - nice */
};

/**
 * @brief Linear filtering is nicer but slower.
*/
enum class TextureMinFilterInMipmap : TextureFlagsType {
	NEAREST,								/**< Nearest pixel - sharp */
	LINEAR = TEX_MIN_FILTER_IN_MIPMAP_BITS	/**< Linear interpolation of pixels - smooth */
};

/**
 * @brief Linear filtering is nicer but slower.
 * Irrelevant if not using mipmaps.
*/
enum class TextureMinFilterBetweenMipmaps : TextureFlagsType {
	NEAREST,									/**< Nearest pixel - sharp */
	LINEAR = TEX_MIN_FILTER_BETWEEN_MIPMAPS_BITS/**< Linear interpolation of pixels - smooth */
};

/**
 * @brief Minifying filter specifies usage of mipmaps and nearest/linear interpolation.
 * Mipmaps are auto-generated minified version of original image.
 * Linear & mipmaps look better but they are slower.
*/
enum class TextureMinFilter : TextureFlagsType {
	NEAREST,								/**< Nearest in the single image, no mipmaps*/
	LINEAR = TEX_MIN_FILTER_IN_MIPMAP_BITS,	/**< Linear in the single image, no mipmaps*/
	NEAREST_MIPMAP_NEAREST = TEX_MIN_FILTER_MIPMAPS_USAGE_BITS,/**< Nearest in mipmap, nearest between mipmaps*/
	NEAREST_MIPMAP_LINEAR = TEX_MIN_FILTER_MIPMAPS_USAGE_BITS | TEX_MIN_FILTER_BETWEEN_MIPMAPS_BITS,/**< Nearest in mipmap, linear between mipmaps*/
	LINEAR_MIPMAP_NEAREST = TEX_MIN_FILTER_MIPMAPS_USAGE_BITS | TEX_MIN_FILTER_IN_MIPMAP_BITS,/**< Linear in mipmap, nearest between mipmaps*/
	LINEAR_MIPMAP_LINEAR = TEX_MIN_FILTER_MIPMAPS_USAGE_BITS | TEX_MIN_FILTER_IN_MIPMAP_BITS | TEX_MIN_FILTER_BETWEEN_MIPMAPS_BITS/**< Linear in mipmap, linear between mipmaps*/
};

/**
 * @brief Magnifying filter specifies interpolation.
 * Linear produces smoother results but it is slower.
*/
enum class TextureMagFilter : TextureFlagsType {
	NEAREST,					/**< Nearest pixel - sharp */
	LINEAR = TEX_MAG_FILTER_BITS/**< Linear interpolation of pixels - smooth */
};

static_assert(TEX_CLAMP_STYLE_X_BITS == TEX_REPEAT_STYLE_X_BITS, "TextureFlags - clamp/repeat style bits rearranged");

/**
 * @brief Wrap style specifies what is outside the raster of texture.
 * This can be different in X and Y axis.
*/
enum class TextureWrapStyle : TextureFlagsType {
	CLAMP_TO_EDGE,								/**< Pixels on the edge are infinitely extended */
	CLAMP_TO_BORDER = TEX_CLAMP_STYLE_X_BITS,	/**< Constant border colour is outside the raster */
	REPEAT_NORMALLY = TEX_WRAP_STYLE_X_BITS,	/**< Texture is repeated infinitely */
	REPEAT_MIRRORED = TEX_WRAP_STYLE_X_BITS | TEX_REPEAT_STYLE_X_BITS /**< Texture is repeated infinitely, mirrored */
};

/**
 * @brief Flags control how the texture gets drawn.
 *
 * What flags control: number of channels, internal format,
 * minifying filter, magnifying filter, wrapping style.
*/
class TextureFlags {
public:
	static const TextureFlagsType RGBA_NU_NEAR_NEAR_EDGE;/**< RGBA, normalized - unsigned, near min&mag filter, clamping to edge */
	static const TextureFlagsType RGBA_IU_NEAR_NEAR_EDGE;/**< RGBA, integral - unsigned, near min&mag filter, clamping to edge */

	/**
	 * @brief Default constructs flags to all zeros
	*/
	TextureFlags() : p_flags(0) {}

	/**
	 * @brief Conversion from raw type (no validity check!)
	*/
	TextureFlags(TextureFlagsType flags) : p_flags(flags) {}

	/**
	 * @brief Constructs flags from all the settings
	*/
	TextureFlags(TextureChannels channels, TextureFormat format,
		TextureMinFilter minFilter, TextureMagFilter magFilter,
		TextureWrapStyle wrapStyleX, TextureWrapStyle wrapStyleY);

	/**
	 * @brief Conversion to raw type
	*/
	explicit operator TextureFlagsType() const { return p_flags; }

	TextureChannels getChannels() const;
	TextureFormatType getFormatType() const;
	TextureFormatSign getFormatSign() const;
	TextureFormat getFormat() const;
	TextureMinFilterMipmapsUsage getMinFilterMipmapsUsage() const;
	TextureMinFilterInMipmap getMinFilterInMipmap() const;
	TextureMinFilterBetweenMipmaps getMinFilterBetweenMipmaps() const;
	TextureMinFilter getMinFilter() const;
	TextureMagFilter getMagFilter() const;
	TextureWrapStyle getWrapStyleX() const;
	TextureWrapStyle getWrapStyleY() const;

	void setChannels(TextureChannels channels);
	void setFormat(TextureFormat format);
	void setFormat(TextureFormatType type, TextureFormatSign sign);
	void setMinFilter(TextureMinFilter minFilter);
	void setMagFilter(TextureMagFilter magFilter);
	void setWrapStyleX(TextureWrapStyle wrapStyle);
	void setWrapStyleY(TextureWrapStyle wrapStyle);
protected:
	TextureFlagsType p_flags;/**< Raw integer type storing the flags */
};

}