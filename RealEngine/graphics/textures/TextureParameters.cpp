/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/textures/TextureParameters.hpp>

#include <fstream>
#include <cstring>

#include <RealEngine/main/Error.hpp>
#include <RealEngine/utility/endianness.hpp>


namespace glm {
	using ivec2_32 = glm::vec<2, int32_t>;
	using uvec2_32 = glm::vec<2, uint32_t>;
}

namespace RE {

	struct RealTextureInformation {
		uint16_t version;
		TextureFlagsType flags;
		glm::ivec2_32 pivot;
		glm::uvec2_32 subimagesSpritesCount;
		glm::uvec2_32 subimageDims;
		RE::Color borderColor;

		void toHost() {
			ntohBulk(version, flags,
				pivot.x, pivot.y, subimagesSpritesCount.x, subimagesSpritesCount.y,
				subimageDims.x, subimageDims.y);
		}

		void toNetwork() {
			htonBulk(version, flags,
				pivot.x, pivot.y, subimagesSpritesCount.x, subimagesSpritesCount.y,
				subimageDims.x, subimageDims.y);
		}
	};
	static_assert(sizeof(RealTextureInformation) == 32, "sizeof(RealTextureInformation) != 32 bytes");

	TextureParameters::TextureParameters(const unsigned char* rtiBytes, size_t size) {
		if (size < sizeof(RealTextureInformation)) {
			throw "Not enough bytes for rti";
		}
		RealTextureInformation rti;
		std::memcpy(&rti, rtiBytes, sizeof(RealTextureInformation));

		//Convert RTI to host endianness
		rti.toHost();

		//Decode RTI
		if (rti.version != 400) {
			throw "Unsupported version of rti";
		}
		m_flags = rti.flags;
		m_subimageDims = glm::dvec2{ rti.subimageDims } + 1.0;
		m_pivot = glm::dvec2{ rti.pivot } / 2.0;
		m_subimagesSpritesCount = glm::dvec2{ rti.subimagesSpritesCount } + 1.0;
		m_definedByRaster = false;
		m_borderColor = rti.borderColor;
	}

	std::vector<unsigned char> TextureParameters::convertToRTI() const {
		//Encode RTI
		RealTextureInformation rti;
		rti.version = 400;
		rti.flags = m_flags;
		rti.subimageDims = glm::ivec2_32{ m_subimageDims } - 1;
		rti.pivot = glm::ivec2_32{ m_pivot * 2.0f};
		rti.subimagesSpritesCount = glm::ivec2_32{ m_subimagesSpritesCount } - 1;
		rti.borderColor = m_borderColor;

		//Convert RTI to network endianness
		rti.toNetwork();

		std::vector<unsigned char> bytes;
		bytes.resize(sizeof(RealTextureInformation));
		std::memcpy(bytes.data(), &rti, sizeof(RealTextureInformation));
		return bytes;
	}

}