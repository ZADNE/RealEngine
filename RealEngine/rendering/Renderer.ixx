/*!
 *  @author    Dubsky Tomas
 */
export module RealEngine.rendering.Renderer;

export namespace RE {

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

class GL46Buffer;
class GL46Capabilities;
class GL46Framebuffer;
class GL46Ordering;
class GL46ShaderProgram;
class GL46Texture;
class GL46VertexArray;
class GL46Viewport;

/**
 * @brief Is renderer type collection that uses OpenGL 4.6
 *
 * This is one of the possible types that can be passed as renderer type
 * to your early-bind Room-template
*/
class RendererGL46 {
public:

    using Buffer = GL46Buffer;
    using Capabilities = GL46Capabilities;
    using Framebuffer = GL46Framebuffer;
    using Ordering = GL46Ordering;
    using ShaderProgram = GL46ShaderProgram;
    using Texture = GL46Texture;
    using VertexArray = GL46VertexArray;
    using Viewport = GL46Viewport;
};

template<class T>
concept Renderer = requires {
    typename T::Buffer;
    typename T::Capabilities;
    typename T::Framebuffer;
    typename T::Ordering;
    typename T::ShaderProgram;
    typename T::Texture;
    typename T::VertexArray;
    typename T::Viewport;
};

}