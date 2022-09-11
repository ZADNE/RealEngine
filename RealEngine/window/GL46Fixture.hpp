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

#include <RealEngine/rendering/output/Framebuffer.hpp>
#include <RealEngine/rendering/Renderer.hpp>

namespace RE {

/**
* @brief Enforces use of OpenGL 4.6 graphics backend.
*
* This is used internally when the RealEngine starts.
*
* @warning Never use this class directly!
*/
class GL46Fixture {
public:

    /**
    * @brief Tries to prepare for creation of OpenGL 4.6 context.
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

    GL46Fixture(const GL46Fixture&) = delete;
    GL46Fixture& operator=(const GL46Fixture&) = delete;

private:

    GL46Fixture();
    ~GL46Fixture();

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

    Framebuffer<RendererLateBind> m_defaultFramebufferLateBind;
    Framebuffer<RendererGL46> m_defaultFramebufferGL46;
};

/**
* @brief Polls all OpenGL errors and logs them as RealEngine errors.
*/
void checkForOpenGLErrors();

}