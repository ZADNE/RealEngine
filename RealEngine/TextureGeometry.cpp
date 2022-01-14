#include <RealEngine/TextureGeometry.hpp>

namespace RE {

	TextureGeometry::TextureGeometry(const glm::vec2& dims, const glm::vec2& pivot/* = {0.0f, 0.0f}*/) :
		p_subimageDims(dims), p_pivot(pivot), p_subimagesSpritesCount(1.0f, 1.0f) {

	}

	TextureGeometry::TextureGeometry(const glm::vec2& subimageDims, const glm::vec2& pivot, const glm::vec2& subimagesSpritesCount) :
		p_subimageDims(subimageDims), p_pivot(pivot), p_subimagesSpritesCount(subimagesSpritesCount) {

	}

}