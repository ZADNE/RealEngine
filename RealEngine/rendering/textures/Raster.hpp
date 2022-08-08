/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vector>

#include <glm/vec2.hpp>

#include <RealEngine/rendering/textures/TextureParameters.hpp>

namespace RE {

	/**
	 * @brief Is a rectangular grid of texels 8-bit per channel, 1 to 4 channels
	*/
	class Raster {
	public:
		/**
		 * @brief Constructs raster without specifying the texels.
		 * @param dims Dimensions of the raster
		 * @param channels Channels of the raster
		*/
		Raster(const glm::uvec2& dims, TextureChannels channels = TextureChannels::RGBA);

		/**
		 * @brief Constructs raster including its texels.
		 *
		 * Pixels must be stored in row major, top-to-bottom, no padding order, 8-bits per channel.
		 * At least (dims.x * dims.y * numberOfChannels) texels must be provided. Following texels are ingored
		 *
		 * @param dims Dimensions of the raster
		 * @param channels Channels of the raster and the provided texels
		 * @param texels Pixels of the raster, size must be at least: (dims.x * dims.y * numberOfChannels)
		*/
		Raster(const glm::uvec2& dims, TextureChannels channels, const std::vector<unsigned char>& texels);

		/**
		 * @brief Gets dimensions of the raster
		 * @return Dimensions of the raster
		*/
		glm::uvec2 getDims() const { return m_dims; }

		/**
		 * @brief Gets texels of the raster
		 * @return Pixels of the raster
		*/
		const std::vector<unsigned char>& getTexels() const { return m_texels; }

		/**
		 * @brief Gets channel format of the raster
		 * @return Channels of the raster
		*/
		TextureChannels getChannels() const { return m_channels; }

		/**
		 * @brief Calculates minimum memory that WOULD BE required to store the raster.
		 *
		 * That is: (dims.x * dims.y * numberOfChannels).
		 *
		 * Note that this function does NOT return actual number of bytes allocated.
		 *
		 * @return Memory requirements of the raster
		*/
		size_t requiredMemory() const;

		/**
		 * @see requiredMemory()
		*/
		static size_t requiredMemory(const glm::uvec2& dims, TextureChannels channels);

	private:
		glm::uvec2 m_dims;/**< dimensions of the raster */
		std::vector<unsigned char> m_texels;/**< texels of the raster, may be empty if they are not specified */
		TextureChannels m_channels;/**< format of the raster */
	};

}