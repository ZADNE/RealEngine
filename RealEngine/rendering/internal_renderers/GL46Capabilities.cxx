/*!
 *  @author    Dubsky Tomas
 */
module;
#include <GL/glew.h>

module RealEngine.rendering.internal_renderers.GL46Capabilities;
import std;
import RealEngine.utility.Error;

namespace RE {

GLenum convert(Capability cap) {
    switch (cap) {
    case Capability::BLENDING: return GL_BLEND;
    default: fatalError("Capability error with unexpected value: " + std::to_string(static_cast<int>(cap)));
    }
}

void GL46Capabilities::set(Capability cap, bool enabled) const {
    auto cap_gl = convert(cap);
    if (enabled) {
        glEnable(cap_gl);
    } else {
        glDisable(cap_gl);
    }
}

}