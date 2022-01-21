#pragma once
#include <vector>

#include <glm/vec2.hpp>

#include <RealEngine/graphics/texture/TextureParameters.hpp>

namespace RE {

	/**
	 * @brief Image is a rectangular raster of pixels 8-bit per channel, 1-4 channels
	*/
	class TextureImage {
	public:
		/**
		 * @brief Constructs image without specifying the pixels.
		 * @param dims Dimensions of the image
		 * @param channels Channels of the image
		*/
		TextureImage(const glm::uvec2& dims, TextureChannels channels = TextureChannels::RGBA);

		/**
		 * @brief Constructs image including its pixels.
		 *
		 * Pixels must be stored in row major, top-to-bottom, no padding order, 8-bits per channel.
		 * At least (dims.x * dims.y * numberOfChannels) pixels must be provided. Following pixels are ingored
		 *
		 * @param dims Dimensions of the image
		 * @param channels Channels of the image and the provided pixels
		 * @param pixels Pixels of the image, size must be at least: (dims.x * dims.y * numberOfChannels)
		*/
		TextureImage(const glm::uvec2& dims, TextureChannels channels, const std::vector<unsigned char>& pixels);

		/**
		 * @brief Gets dimensions of the image
		 * @return Dimensions of the image
		*/
		glm::uvec2 getDims() const { return m_dims; }

		/**
		 * @brief Gets pixels of the image
		 * @return Pixels of the image
		*/
		const std::vector<unsigned char>& getPixels() const { return m_pixels; }

		/**
		 * @brief Gets channel format of the image
		 * @return Channels of the image
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
		glm::uvec2 m_dims;/**< dimensions of the image */
		std::vector<unsigned char> m_pixels;/**< pixels of the image, may be empty if they are not specified */
		TextureChannels m_channels;/**< format of the image */
	};

}