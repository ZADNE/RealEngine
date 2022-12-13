/*!
 *  @author    Dubsky Tomas
 */
#pragma once

namespace RE {

class IBuffer;
class ICapabilities;
class IDescriptorSet;
class IFramebuffer;
class IOrdering;
class IPipeline;
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
    using DescriptorSet = IDescriptorSet;
    using Framebuffer = IFramebuffer;
    using Ordering = IOrdering;
    using Pipeline = IPipeline;
    using ShaderProgram = IShaderProgram;
    using Texture = ITexture;
    using VertexArray = IVertexArray;
    using Viewport = IViewport;
};

class VK13Buffer;
class VK13Capabilities;
class VK13DescriptorSet;
class VK13Framebuffer;
class VK13Ordering;
class VK13Pipeline;
class VK13ShaderProgram;
class VK13Texture;
class VK13VertexArray;
class VK13Viewport;

/**
 * @brief Is renderer type collection that uses Vulkan 1.3
*/
class RendererVK13 {
public:

    using Buffer = VK13Buffer;
    using Capabilities = VK13Capabilities;
    using DescriptorSet = VK13DescriptorSet;
    using Framebuffer = VK13Framebuffer;
    using Ordering = VK13Ordering;
    using Pipeline = VK13Pipeline;
    using ShaderProgram = VK13ShaderProgram;
    using Texture = VK13Texture;
    using VertexArray = VK13VertexArray;
    using Viewport = VK13Viewport;
};


template<class T>
concept Renderer = requires {
    typename T::Buffer;
    typename T::Capabilities;
    typename T::DescriptorSet;
    typename T::Framebuffer;
    typename T::Ordering;
    typename T::Pipeline;
    typename T::ShaderProgram;
    typename T::Texture;
    typename T::VertexArray;
    typename T::Viewport;
};

}