/*! 
 *  @author    Dubsky Tomas
 *    @file
 */
#pragma once
#include <RealEngine/rendering/textures/TextureFlags.hpp>

namespace RE {

/**
 * @brief Converts TextureChannels to string representation
*/
inline std::string_view to_string(TextureChannels channels) {
    switch (channels) {
    case TextureChannels::R: return "R";
    case TextureChannels::RG: return "RG";
    case TextureChannels::RGB: return "RGB";
    case TextureChannels::RGBA: return "RGBA";
    default: return "<UNKNOWN TextureChannels>";
    }
}

/**
 * @brief Converts TextureFormat to string representation
*/
inline std::string_view to_string(TextureFormat format) {
    switch (format) {
    case TextureFormat::NORMALIZED_UNSIGNED: return "NORMALIZED_UNSIGNED";
    case TextureFormat::NORMALIZED_SIGNED: return "NORMALIZED_SIGNED";
    case TextureFormat::INTEGRAL_UNSIGNED: return "INTEGRAL_UNSIGNED";
    case TextureFormat::INTEGRAL_SIGNED: return "INTEGRAL_SIGNED";
    default: return "<UNKNOWN TextureFormat>";
    }
}

/**
 * @brief Converts TextureMinFilter to string representation
*/
inline std::string_view to_string(TextureMinFilter minFilter) {
    switch (minFilter) {
    case TextureMinFilter::NEAREST_NO_MIPMAPS: return "NEAREST_NO_MIPMAPS";
    case TextureMinFilter::LINEAR_NO_MIPMAPS: return "LINEAR_NO_MIPMAPS";
    case TextureMinFilter::NEAREST_MIPMAP_NEAREST: return "NEAREST_MIPMAP_NEAREST";
    case TextureMinFilter::NEAREST_MIPMAP_LINEAR: return "NEAREST_MIPMAP_LINEAR";
    case TextureMinFilter::LINEAR_MIPMAP_NEAREST: return "LINEAR_MIPMAP_NEAREST";
    case TextureMinFilter::LINEAR_MIPMAP_LINEAR: return "LINEAR_MIPMAP_LINEAR";
    default: return "<UNKNOWN TextureMinFilter>";
    }
}

/**
 * @brief Converts TextureMagFilter to string representation
*/
inline std::string_view to_string(TextureMagFilter magFilter) {
    switch (magFilter) {
    case TextureMagFilter::NEAREST: return "NEAREST";
    case TextureMagFilter::LINEAR: return "LINEAR";
    default: return "<UNKNOWN TextureMagFilter>";
    }
}

/**
 * @brief Converts TextureWrapStyle to string representation
*/
inline std::string_view to_string(TextureWrapStyle wrapStyle) {
    switch (wrapStyle) {
    case TextureWrapStyle::CLAMP_TO_EDGE: return "CLAMP_TO_EDGE";
    case TextureWrapStyle::CLAMP_TO_BORDER: return "CLAMP_TO_BORDER";
    case TextureWrapStyle::REPEAT_NORMALLY: return "REPEAT_NORMALLY";
    case TextureWrapStyle::REPEAT_MIRRORED: return "REPEAT_MIRRORED";
    default: return "<UNKNOWN TextureWrapStyle>";
    }
}

}