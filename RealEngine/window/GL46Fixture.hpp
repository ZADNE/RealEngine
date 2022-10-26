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

typedef void* SDL_GLContext;
struct SDL_Window;

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
     * @brief Sets up for OpenGL 4.6 rendering
     * @throws If anything fails
    */
    GL46Fixture(SDL_Window* sdlWindow);

    GL46Fixture(const GL46Fixture&) = delete;
    GL46Fixture& operator=(const GL46Fixture&) = delete;

    ~GL46Fixture();

    void prepareFrame(const glm::vec4& clearColor, bool useImGui);
    void finishFrame(bool useImGui, SDL_Window* sdlWindow);

private:

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

    SDL_GLContext m_glContext;

    Implementations m_impls;

    Framebuffer<RendererLateBind> m_defaultFramebufferLateBind;
    Framebuffer<RendererGL46> m_defaultFramebufferGL46;

    SDL_GLContext createContext(SDL_Window* sdlWindow);
};

/**
* @brief Polls all OpenGL errors and logs them as RealEngine errors.
*/
void checkForOpenGLErrors();

}