/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_renderers/GL46Buffer.hpp>
#include <RealEngine/rendering/internal_renderers/GL46Capabilities.hpp>
#include <RealEngine/rendering/internal_renderers/GL46Framebuffer.hpp>
#include <RealEngine/rendering/internal_renderers/GL46Ordering.hpp>
#include <RealEngine/rendering/internal_renderers/GL46ShaderProgram.hpp>
#include <RealEngine/rendering/internal_renderers/GL46Texture.hpp>
#include <RealEngine/rendering/internal_renderers/GL46VertexArray.hpp>
#include <RealEngine/rendering/internal_renderers/GL46Viewport.hpp>

namespace RE {

/**
* @brief Enforces use of Vulkan 1.3 graphics backend.
*
* This is used internally when the RealEngine starts.
*
* @warning Never use this class directly!
*/
class VK13Fixture {
public:

    /**
    * @brief Tries to prepare for creation of Vulkan 1.3 context.
    * @return True if succeeded.
    * @warning Do not call use() if this fails.
    *
    * Requires SDL2 to be initialized.
    *
    * @note To be called only once at the start of the program.
    */
    static bool prepare();

    /**
    * @brief Initializes the renderer.
    * @warning Do not call this if prepare() has failed.
    */
    static void initialize();

    VK13Fixture(const VK13Fixture&) = delete;
    VK13Fixture& operator=(const VK13Fixture&) = delete;

private:

    VK13Fixture();
    ~VK13Fixture();

    class Implementations {
    public:

        Implementations() {
            assignReferences<RendererLateBind>();
            assignReferences<RendererGL46>();
        }

        ~Implementations() {
            clearReferences<RendererLateBind>();
            clearReferences<RendererGL46>();
        }

        Implementations(const Implementations&) = delete;
        Implementations& operator=(const Implementations&) = delete;

    private:

        template<Renderer R>
        void assignReferences();

        template<Renderer R>
        void clearReferences();

        GL46Buffer m_bufferImpl;
        GL46Capabilities m_capabilitiesImpl;
        GL46Framebuffer m_mainFramebufferImpl;
        GL46Ordering m_orderingImpl;
        GL46ShaderProgram m_shaderProgramImpl;
        GL46Texture m_textureImpl;
        GL46VertexArray m_vertexArrayImpl;
        GL46Viewport m_viewportImpl;

        ViewportState m_viewportState;
    };

    Implementations s_impls;
};

}