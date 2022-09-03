/*!
 *  @author    Dubsky Tomas
 */
#pragma once

namespace RE {

class GL46_Buffer;
class GL46_Capabilities;
class GL46_Framebuffer;
class GL46_Ordering;
class GL46_ShaderProgram;
class GL46_Texture;
class GL46_VertexArray;
class GL46_Viewport;

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