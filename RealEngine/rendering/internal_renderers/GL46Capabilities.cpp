/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/GL46Capabilities.hpp>

#include <string>

#include <GL/glew.h>

#include <RealEngine/utility/error.hpp>

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