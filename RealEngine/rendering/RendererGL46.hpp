/*!
 *  @author    Dubsky Tomas
 */
#pragma once

#include <RealEngine/rendering/internal_renderers/GL46_Buffer.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_Capabilities.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_Framebuffer.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_Ordering.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_ShaderProgram.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_Texture.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_VertexArray.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_Viewport.hpp>

namespace RE {

/**
 * @brief Is renderer type collection that uses OpenGL 4.6
 *
 * This is one of the possible types that can be passed as renderer type
 * to your early-bind Room-template
*/
class RendererGL46 {
public:

    using Buffer = GL46_Buffer;
    using Capabilities = GL46_Capabilities;
    using Framebuffer = GL46_Framebuffer;
    using Ordering = GL46_Ordering;
    using ShaderProgram = GL46_ShaderProgram;
    using Texture = GL46_Texture;
    using VertexArray = GL46_VertexArray;
    using Viewport = GL46_Viewport;

};

}