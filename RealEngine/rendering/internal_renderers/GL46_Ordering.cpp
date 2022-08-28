/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/GL46_Ordering.hpp>

#include <string>

#include <GL/glew.h>

#include <RealEngine/utility/error.hpp>

namespace RE {

GLbitfield convert(IncoherentAccessBarrierFlags barriers) {
    GLbitfield barriers_gl = 0;
    using enum IncoherentAccessBarrierFlags;
    if (barriers & VERTEX_ATTRIB_ARRAY) barriers_gl |= GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT;
    if (barriers & ELEMENT_ARRAY) barriers_gl |= GL_ELEMENT_ARRAY_BARRIER_BIT;
    if (barriers & UNIFORM) barriers_gl |= GL_UNIFORM_BARRIER_BIT;
    if (barriers & TEXTURE_FETCH) barriers_gl |= GL_TEXTURE_FETCH_BARRIER_BIT;
    if (barriers & SHADER_IMAGE_ACCESS) barriers_gl |= GL_SHADER_IMAGE_ACCESS_BARRIER_BIT;
    if (barriers & COMMAND) barriers_gl |= GL_COMMAND_BARRIER_BIT;
    if (barriers & PIXEL_BUFFER) barriers_gl |= GL_PIXEL_BUFFER_BARRIER_BIT;
    if (barriers & TEXTURE_UPDATE) barriers_gl |= GL_TEXTURE_UPDATE_BARRIER_BIT;
    if (barriers & BUFFER_UPDATE) barriers_gl |= GL_BUFFER_UPDATE_BARRIER_BIT;
    if (barriers & FRAMEBUFFER) barriers_gl |= GL_FRAMEBUFFER_BARRIER_BIT;
    if (barriers & TRANSFORM_FEEDBACK) barriers_gl |= GL_TRANSFORM_FEEDBACK_BARRIER_BIT;
    if (barriers & ATOMIC_COUNTER) barriers_gl |= GL_ATOMIC_COUNTER_BARRIER_BIT;
    if (barriers & SHADER_STORAGE) barriers_gl |= GL_SHADER_STORAGE_BARRIER_BIT;
    return barriers_gl;
}

void GL46_Ordering::flushWork() const {
    glFlush();
}

void GL46_Ordering::finishWork() const {
    glFinish();
}

void GL46_Ordering::issueDrawBarrier() const {
    glTextureBarrier();
}

void GL46_Ordering::issueIncoherentAccessBarrier(IncoherentAccessBarrierFlags barriers) const {
    auto barriers_gl = convert(barriers);
    glMemoryBarrier(barriers_gl);
}

}