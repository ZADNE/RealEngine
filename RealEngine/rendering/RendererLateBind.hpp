/*!
 *  @author    Dubsky Tomas
 */
#pragma once

#include <RealEngine/rendering/internal_interfaces/IBuffer.hpp>
#include <RealEngine/rendering/internal_interfaces/ICapabilities.hpp>
#include <RealEngine/rendering/internal_interfaces/IFramebuffer.hpp>
#include <RealEngine/rendering/internal_interfaces/IOrdering.hpp>
#include <RealEngine/rendering/internal_interfaces/IShaderProgram.hpp>
#include <RealEngine/rendering/internal_interfaces/ITexture.hpp>
#include <RealEngine/rendering/internal_interfaces/IVertexArray.hpp>
#include <RealEngine/rendering/internal_interfaces/IViewport.hpp>

namespace RE {

/**
 * @brief Is renderer type collection that polymorphically translates call to backing renderer
 *
 * This renderer is used as the default renderer if you do not pass
 * the renderer type from your early-bind Room.
*/
class RendererLateBind {
public:

    using Buffer = IBuffer;
    using Capabilities = ICapabilities;
    using Framebuffer = IFramebuffer;
    using Ordering = IOrdering;
    using ShaderProgram = IShaderProgram;
    using Texture = ITexture;
    using VertexArray = IVertexArray;
    using Viewport = IViewport;

};

}