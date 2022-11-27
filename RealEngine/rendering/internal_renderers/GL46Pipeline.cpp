/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/GL46Pipeline.hpp>

#include <GL/glew.h>

#include <RealEngine/utility/Error.hpp>

namespace RE {

static int convertToComponentCount(vk::Format format) {
    using enum vk::Format;
    switch (format) {
    case eR8Uint: case eR16Uint: case eR32Uint:
    case eR8Unorm: case eR16Unorm:
    case eR8Uscaled: case eR16Uscaled:
    case eR32Sfloat:
        return 1;
    case eR8G8Uint: case eR16G16Uint: case eR32G32Uint:
    case eR8G8Unorm: case eR16G16Unorm:
    case eR8G8Uscaled: case eR16G16Uscaled:
    case eR32G32Sfloat:
        return 2;
    case eR8G8B8Uint: case eR16G16B16Uint: case eR32G32B32Uint:
    case eR8G8B8Unorm: case eR16G16B16Unorm:
    case eR8G8B8Uscaled: case eR16G16B16Uscaled:
    case eR32G32B32Sfloat:
        return 3;
    case eR8G8B8A8Uint: case eR16G16B16A16Uint: case eR32G32B32A32Uint:
    case eR8G8B8A8Unorm: case eR16G16B16A16Unorm:
    case eR8G8B8A8Uscaled: case eR16G16B16A16Uscaled:
    case eR32G32B32A32Sfloat:
        return 4;
    default:
        throw Exception{"Unsupported vk::Format"};
    }
}

static GLenum convertToComponentType(vk::Format format) {
    using enum vk::Format;
    switch (format) {
    case eR8Uint: case eR8Unorm: case eR8Uscaled:
    case eR8G8Uint: case eR8G8Unorm: case eR8G8Uscaled:
    case eR8G8B8Uint: case eR8G8B8Unorm: case eR8G8B8Uscaled:
    case eR8G8B8A8Uint: case eR8G8B8A8Unorm: case eR8G8B8A8Uscaled:
        return GL_UNSIGNED_BYTE;
    case eR16Uint: case eR16Unorm: case eR16Uscaled:
    case eR16G16Uint: case eR16G16Unorm: case eR16G16Uscaled:
    case eR16G16B16Uint: case eR16G16B16Unorm: case eR16G16B16Uscaled:
    case eR16G16B16A16Uint: case eR16G16B16A16Unorm: case eR16G16B16A16Uscaled:
        return GL_UNSIGNED_SHORT;
    case eR32Uint:
    case eR32G32Uint:
    case eR32G32B32Uint:
    case eR32G32B32A32Uint:
        return GL_UNSIGNED_INT;
    case eR32Sfloat:
    case eR32G32Sfloat:
    case eR32G32B32Sfloat:
    case eR32G32B32A32Sfloat:
        return GL_FLOAT;
    default:
        throw Exception{"Unsupported vk::Format"};
    }
}

static bool needsNormalization(vk::Format format) {
    using enum vk::Format;
    switch (format) {
    case eR8Unorm: case eR16Unorm:
    case eR8G8Unorm: case eR16G16Unorm:
    case eR8G8B8Unorm: case eR16G16B16Unorm:
    case eR8G8B8A8Unorm: case eR16G16B16A16Unorm:
        return true;
    default:
        return false;
    }
}

PipelineID GL46Pipeline::construct(/*const std::span<const BufferID>& vbs, */const vk::PipelineVertexInputStateCreateInfo& vi, const ShaderProgramSources& sources) const {
    auto va = createVertexArray(/*vbs*/{}, vi);
    auto sp = createShaderProgram(sources);
    return PipelineID::GL46{.va = va, .sp = sp};
}

void GL46Pipeline::destruct(PipelineID& pl) const {
    glDeleteProgram(pl.m_.gl.sp);
    glDeleteVertexArrays(1, &pl.m_.gl.va);
}

unsigned int GL46Pipeline::createVertexArray(
    const std::span<const BufferID>& vbs,
    const vk::PipelineVertexInputStateCreateInfo& vi) const
{
    unsigned int va = 0;
    glCreateVertexArrays(1, &va);
    //Attributes
    for (size_t i = 0; i < vi.vertexAttributeDescriptionCount; i++) {
        auto attr = vi.pVertexAttributeDescriptions[i];
        glEnableVertexArrayAttrib(va, attr.location);
        auto componentCount = convertToComponentCount(attr.format);
        auto componentType = convertToComponentType(attr.format);
        if (needsNormalization(attr.format)) {
            glVertexArrayAttribFormat(va, attr.location, componentCount, componentType, true, attr.offset);
        } else {
            glVertexArrayAttribIFormat(va, attr.location, componentCount, componentType, attr.offset);
        }
        glVertexArrayAttribBinding(va, attr.location, attr.binding);
    }
    //Bindings
    for (size_t i = 0; i < vi.vertexBindingDescriptionCount; i++) {
        auto bndg = vi.pVertexBindingDescriptions[i];
        //glVertexArrayVertexBuffer(va, bndg.binding, vbs[bndg.binding].m_id, offset, bndg.stride);
    }
    return va;
}

unsigned int GL46Pipeline::createShaderProgram(const ShaderProgramSources& sources) const {
    return 0;
}

}