#pragma once

#include <RealEngine/TextureFlags.hpp>

namespace RE {

	/**
	 * @brief Converts TextureChannels to string representation
	*/
	inline const char* to_string(TextureChannels channels) {
		switch (channels) {
		case RE::TextureChannels::R: return "R";
		case RE::TextureChannels::RG: return "RG";
		case RE::TextureChannels::RGB: return "RGB";
		case RE::TextureChannels::RGBA: return "RGBA";
		default: return "<UNKNOWN RE::TextureChannels>";
		}
	}

	/**
	 * @brief Converts TextureFormat to string representation
	*/
	inline const char* to_string(TextureFormat format) {
		switch (format) {
		case RE::TextureFormat::NORMALIZED_UNSIGNED: return "NORMALIZED_UNSIGNED";
		case RE::TextureFormat::NORMALIZED_SIGNED: return "NORMALIZED_SIGNED";
		case RE::TextureFormat::INTEGRAL_UNSIGNED: return "INTEGRAL_UNSIGNED";
		case RE::TextureFormat::INTEGRAL_SIGNED: return "INTEGRAL_SIGNED";
		default: return "<UNKNOWN RE::TextureFormat>";
		}
	}

	/**
	 * @brief Converts TextureMinFilter to string representation
	*/
	inline const char* to_string(TextureMinFilter minFilter) {
		switch (minFilter) {
		case RE::TextureMinFilter::NEAREST: return "NEAREST";
		case RE::TextureMinFilter::LINEAR: return "LINEAR";
		case RE::TextureMinFilter::NEAREST_MIPMAP_NEAREST: return "NEAREST_MIPMAP_NEAREST";
		case RE::TextureMinFilter::NEAREST_MIPMAP_LINEAR: return "NEAREST_MIPMAP_LINEAR";
		case RE::TextureMinFilter::LINEAR_MIPMAP_NEAREST: return "LINEAR_MIPMAP_NEAREST";
		case RE::TextureMinFilter::LINEAR_MIPMAP_LINEAR: return "LINEAR_MIPMAP_LINEAR";
		default: return "<UNKNOWN RE::TextureMinFilter>";
		}
	}

	/**
	 * @brief Converts TextureMagFilter to string representation
	*/
	inline const char* to_string(TextureMagFilter magFilter) {
		switch (magFilter) {
		case RE::TextureMagFilter::NEAREST: return "NEAREST";
		case RE::TextureMagFilter::LINEAR: return "LINEAR";
		default: return "<UNKNOWN RE::TextureMagFilter>";
		}
	}

	/**
	 * @brief Converts TextureWrapStyle to string representation
	*/
	inline const char* to_string(TextureWrapStyle wrapStyle) {
		switch (wrapStyle) {
		case RE::TextureWrapStyle::CLAMP_TO_EDGE: return "CLAMP_TO_EDGE";
		case RE::TextureWrapStyle::CLAMP_TO_BORDER: return "CLAMP_TO_BORDER";
		case RE::TextureWrapStyle::REPEAT_NORMALLY: return "REPEAT_NORMALLY";
		case RE::TextureWrapStyle::REPEAT_MIRRORED: return "REPEAT_MIRRORED";
		default: return "<UNKNOWN RE::TextureWrapStyle>";
		}
	}

}