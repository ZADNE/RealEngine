#pragma once
#include <string>
#include <vector>

#include <glm/vec2.hpp>

#include <RealEngine/graphics/texture/TextureGeometry.hpp>
#include <RealEngine/graphics/texture/TextureFlags.hpp>
#include <RealEngine/graphics/Vertex.hpp>

namespace RE {
	/**
	 * @brief Texture parameters specify all auxiliary information (except the pixels)
	 * that is required to draw a texture.
	*/
	class TextureParameters : public TextureGeometry, public TextureFlags {
	public:

		/**
		 * @brief Constructs parameters from raw bytes encoded as Real Texture Infromation (RTI)
		 *
		 * @throws When bytes cannot be decoded.
		 *
		 * @param rtiBytes Raw bytes of RTI
		 * @param size Size of the buffer rtiBytes
		*/
		TextureParameters(const unsigned char* rtiBytes, size_t size);

		/**
		 * @brief Constructs parameters from given geometry, flags and border colour
		 * @param geometry Geometry of the parameters
		 * @param flags Flags of the parameters
		 * @param borderColour Border colour of the parameters
		*/
		TextureParameters(const TextureGeometry& geometry, TextureFlags flags, RE::Colour borderColour) :
			TextureGeometry(geometry), TextureFlags(flags), m_borderColour(borderColour) {}

		/**
		 * @brief Constructs parameters from given geometry and flags.
		 *
		 * Colour is set to (0, 0, 0, 255).
		 *
		 * @param geometry Geometry of the parameters
		 * @param flags Flags of the parameters
		*/
		TextureParameters(const TextureGeometry& geometry, TextureFlags flags) :
			TextureParameters(geometry, flags, { 0u, 0u, 0u, 255u }) {}

		/**
		 * @brief Constructs parameters with flags only.
		 *
		 * Geometry is set to be defined by image.
		 * Colour is set to (0, 0, 0, 255).
		 *
		 * @param flags Flags of the parameters
		*/
		TextureParameters(TextureFlags flags) :
			TextureParameters(TextureGeometry{}, flags) {}

		/**
		 * @brief Gets the border colour.
		 *
		 * Border colour is the colour that will be sampled outside texture's texels
		 * if clamping to border is used. It has no effect if clamping to border
		 * is not used.
		 * @return Border colour of the texture
		*/
		RE::Colour getBorderColour() const { return m_borderColour; }

		/**
		 * @brief Converts parameters to raw bytes, encoded as Real Texture Infromation (RTI)
		 * @return Raw bytes of RTI that represent these parameters
		*/
		std::vector<unsigned char> convertToRTI() const;
	private:
		RE::Colour m_borderColour;/**< Border colour of the texture*/
	};

}