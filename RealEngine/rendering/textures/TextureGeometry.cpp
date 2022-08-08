/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/textures/TextureGeometry.hpp>

namespace RE {

	TextureGeometry::TextureGeometry(const glm::vec2& dims, const glm::vec2& pivot/* = {0.0f, 0.0f}*/) :
		m_subimageDims(dims), m_pivot(pivot), m_subimagesSpritesCount(1.0f, 1.0f) {

	}

	TextureGeometry::TextureGeometry(const glm::vec2& subimageDims, const glm::vec2& pivot, const glm::vec2& subimagesSpritesCount) :
		m_subimageDims(subimageDims), m_pivot(pivot), m_subimagesSpritesCount(subimagesSpritesCount) {

	}

}