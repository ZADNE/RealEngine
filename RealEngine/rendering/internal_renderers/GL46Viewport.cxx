/*! 
 *  @author    Dubsky Tomas
 */
module;
#include <GL/glew.h>

module RealEngine.rendering.internal_renderers.GL46Viewport;

namespace RE {

void GL46Viewport::set(const glm::ivec2& pos, const glm::ivec2& size) const {
    glViewport(pos.x, pos.y, size.x, size.y);
}

}