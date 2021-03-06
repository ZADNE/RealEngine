/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/textures/Raster.hpp>

#include <lodepng/lodepng.hpp>

#include <RealEngine/main/Error.hpp>

namespace RE {

	Raster::Raster(const glm::uvec2& dims, TextureChannels channels/* = TextureChannels::RGBA*/) :
		m_dims(dims), m_channels(channels) {
		assert(dims.x > 0 && dims.y > 0);
	}

	Raster::Raster(const glm::uvec2& dims, TextureChannels channels, const std::vector<unsigned char>& texels) :
		m_dims(dims), m_texels(texels), m_channels(channels) {
		assert(dims.x > 0 && dims.y > 0);
		assert(requiredMemory() <= texels.size());
	}

	size_t Raster::requiredMemory() const {
		return requiredMemory(m_dims, m_channels);
	}

	size_t Raster::requiredMemory(const glm::uvec2& dims, TextureChannels channels) {
		return static_cast<size_t>(dims.x) * dims.y * (static_cast<size_t>(channels) + 1);
	}

}