#include <RealEngine/graphics/texture/TextureImage.hpp>

#include <RealEngine/main/Error.hpp>
#include <RealEngine/external/lodepng/lodepng.hpp>

namespace RE {

	TextureImage::TextureImage(const glm::uvec2& dims, TextureChannels channels/* = TextureChannels::RGBA*/) :
		m_dims(dims), m_channels(channels) {
		assert(dims.x > 0 && dims.y > 0);
	}

	TextureImage::TextureImage(const glm::uvec2& dims, TextureChannels channels, const std::vector<unsigned char>& pixels) :
		m_dims(dims), m_pixels(pixels), m_channels(channels) {
		assert(dims.x > 0 && dims.y > 0);
		assert(requiredMemory() <= pixels.size());
	}

	size_t TextureImage::requiredMemory() const {
		return requiredMemory(m_dims, m_channels);
	}

	size_t TextureImage::requiredMemory(const glm::uvec2& dims, TextureChannels channels) {
		return static_cast<size_t>(dims.x) * dims.y * (static_cast<size_t>(channels) + 1);
	}

}