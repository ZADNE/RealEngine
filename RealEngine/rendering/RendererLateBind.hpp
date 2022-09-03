/*!
 *  @author    Dubsky Tomas
 */
#pragma once

namespace RE {

class IBuffer;
class ICapabilities;
class IFramebuffer;
class IOrdering;
class IShaderProgram;
class ITexture;
class IVertexArray;
class IViewport;

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