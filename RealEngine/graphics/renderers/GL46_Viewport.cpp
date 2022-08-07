/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/renderers/GL46_Viewport.hpp>

#include <GL/glew.h>

namespace RE {

void GL46_Viewport::set(const glm::ivec2& pos, const glm::ivec2& size) const {
	glViewport(pos.x, pos.y, size.x, size.y);
}

}