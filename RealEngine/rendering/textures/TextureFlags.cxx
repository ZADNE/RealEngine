/*!
 *  @author    Dubsky Tomas
 */
module RealEngine.rendering.textures.TextureFlags;

namespace RE {

TextureChannels TextureFlags::getChannels() const {
    return static_cast<TextureChannels>(m_flags & TEX_CHANNELS_BITS);
}

TextureFormatType TextureFlags::getFormatType() const {
    return static_cast<TextureFormatType>(m_flags & TEX_FORMAT_TYPE_BITS);
}

TextureFormatSign TextureFlags::getFormatSign() const {
    return static_cast<TextureFormatSign>(m_flags & TEX_FORMAT_SIGN_BITS);
}

TextureFormat TextureFlags::getFormat() const {
    return static_cast<TextureFormat>(m_flags & TEX_FORMAT_BITS);
}

TextureMinFilterMipmapsUsage TextureFlags::getMinFilterMipmapsUsage() const {
    return static_cast<TextureMinFilterMipmapsUsage>(m_flags & TEX_MIN_FILTER_MIPMAPS_USAGE_BITS);
}

TextureMinFilterInMipmap TextureFlags::getMinFilterInMipmap() const {
    return static_cast<TextureMinFilterInMipmap>(m_flags & TEX_MIN_FILTER_IN_MIPMAP_BITS);
}

TextureMinFilterBetweenMipmaps TextureFlags::getMinFilterBetweenMipmaps() const {
    return static_cast<TextureMinFilterBetweenMipmaps>(m_flags & TEX_MIN_FILTER_BETWEEN_MIPMAPS_BITS);
}

TextureMinFilter TextureFlags::getMinFilter() const {
    return static_cast<TextureMinFilter>(m_flags & TEX_MIN_FILTER_BITS);
}

TextureMagFilter TextureFlags::getMagFilter() const {
    return static_cast<TextureMagFilter>(m_flags & TEX_MAG_FILTER_BITS);
}

TextureWrapStyle TextureFlags::getWrapStyleX() const {
    return static_cast<TextureWrapStyle>(m_flags & TEX_WRAP_X_BITS);
}

TextureWrapStyle TextureFlags::getWrapStyleY() const {
    return static_cast<TextureWrapStyle>((m_flags >> TEX_WRAP_Y_TO_X_RIGHTSHIFT) & TEX_WRAP_X_BITS);
}

TextureBitdepthPerChannel TextureFlags::getBitdepthPerChannel() const {
    return static_cast<TextureBitdepthPerChannel>(m_flags & TEX_BITDEPTH_PER_CHANNEL_BITS);
}

void TextureFlags::setChannels(TextureChannels channels) {
    m_flags = (m_flags & ~TEX_CHANNELS_BITS) | ft_cast(channels);
}

void TextureFlags::setFormat(TextureFormat format) {
    m_flags = (m_flags & ~TEX_FORMAT_BITS) | ft_cast(format);
}

void TextureFlags::setFormat(TextureFormatType type, TextureFormatSign sign) {
    m_flags = (m_flags & ~TEX_FORMAT_BITS) | ft_cast(type) | ft_cast(sign);
}

void TextureFlags::setMagFilter(TextureMagFilter magFilter) {
    m_flags = (m_flags & ~TEX_MAG_FILTER_BITS) | ft_cast(magFilter);
}

void TextureFlags::setMinFilter(TextureMinFilter minFilter) {
    m_flags = (m_flags & ~TEX_MIN_FILTER_BITS) | ft_cast(minFilter);
}

void TextureFlags::setWrapStyleX(TextureWrapStyle wrapStyle) {
    m_flags = (m_flags & ~TEX_WRAP_X_BITS) | ft_cast(wrapStyle);
}

void TextureFlags::setWrapStyleY(TextureWrapStyle wrapStyle) {
    m_flags = (m_flags & ~TEX_WRAP_Y_BITS) | (ft_cast(wrapStyle) << TEX_WRAP_X_TO_Y_LEFTSHIFT);
}

void TextureFlags::setBitdepthPerChannel(TextureBitdepthPerChannel bitdepth) {
    m_flags = (m_flags & ~TEX_BITDEPTH_PER_CHANNEL_BITS) | ft_cast(bitdepth);
}

std::string_view to_string(TextureChannels channels) {
    switch (channels) {
    case RE::TextureChannels::R: return "R";
    case RE::TextureChannels::RG: return "RG";
    case RE::TextureChannels::RGB: return "RGB";
    case RE::TextureChannels::RGBA: return "RGBA";
    default: return "<UNKNOWN RE::TextureChannels>";
    }
}

std::string_view to_string(TextureFormat format) {
    switch (format) {
    case RE::TextureFormat::NORMALIZED_UNSIGNED: return "NORMALIZED_UNSIGNED";
    case RE::TextureFormat::NORMALIZED_SIGNED: return "NORMALIZED_SIGNED";
    case RE::TextureFormat::INTEGRAL_UNSIGNED: return "INTEGRAL_UNSIGNED";
    case RE::TextureFormat::INTEGRAL_SIGNED: return "INTEGRAL_SIGNED";
    default: return "<UNKNOWN RE::TextureFormat>";
    }
}

std::string_view to_string(TextureMinFilter minFilter) {
    switch (minFilter) {
    case RE::TextureMinFilter::NEAREST_NO_MIPMAPS: return "NEAREST_NO_MIPMAPS";
    case RE::TextureMinFilter::LINEAR_NO_MIPMAPS: return "LINEAR_NO_MIPMAPS";
    case RE::TextureMinFilter::NEAREST_MIPMAP_NEAREST: return "NEAREST_MIPMAP_NEAREST";
    case RE::TextureMinFilter::NEAREST_MIPMAP_LINEAR: return "NEAREST_MIPMAP_LINEAR";
    case RE::TextureMinFilter::LINEAR_MIPMAP_NEAREST: return "LINEAR_MIPMAP_NEAREST";
    case RE::TextureMinFilter::LINEAR_MIPMAP_LINEAR: return "LINEAR_MIPMAP_LINEAR";
    default: return "<UNKNOWN RE::TextureMinFilter>";
    }
}

std::string_view to_string(TextureMagFilter magFilter) {
    switch (magFilter) {
    case RE::TextureMagFilter::NEAREST: return "NEAREST";
    case RE::TextureMagFilter::LINEAR: return "LINEAR";
    default: return "<UNKNOWN RE::TextureMagFilter>";
    }
}

std::string_view to_string(TextureWrapStyle wrapStyle) {
    switch (wrapStyle) {
    case RE::TextureWrapStyle::CLAMP_TO_EDGE: return "CLAMP_TO_EDGE";
    case RE::TextureWrapStyle::CLAMP_TO_BORDER: return "CLAMP_TO_BORDER";
    case RE::TextureWrapStyle::REPEAT_NORMALLY: return "REPEAT_NORMALLY";
    case RE::TextureWrapStyle::REPEAT_MIRRORED: return "REPEAT_MIRRORED";
    default: return "<UNKNOWN RE::TextureWrapStyle>";
    }
}

}