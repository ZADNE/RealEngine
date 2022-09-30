/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vulkan/vulkan.hpp>

#include <RealEngine/rendering/internal_renderers/VK13Buffer.hpp>
#include <RealEngine/rendering/internal_renderers/VK13Capabilities.hpp>
#include <RealEngine/rendering/internal_renderers/VK13Framebuffer.hpp>
#include <RealEngine/rendering/internal_renderers/VK13Ordering.hpp>
#include <RealEngine/rendering/internal_renderers/VK13ShaderProgram.hpp>
#include <RealEngine/rendering/internal_renderers/VK13Texture.hpp>
#include <RealEngine/rendering/internal_renderers/VK13VertexArray.hpp>
#include <RealEngine/rendering/internal_renderers/VK13Viewport.hpp>

struct SDL_Window;

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
    static bool initialize(SDL_Window* sdlWindow);

    VK13Fixture(const VK13Fixture&) = delete;
    VK13Fixture& operator=(const VK13Fixture&) = delete;

private:

    VK13Fixture();
    ~VK13Fixture();

    enum class Initialized {
        COMMAND_BUFFER,
        COMMAND_POOL,
        DEVICE,
        INSTANCE,
        NOTHING
    };

    bool init(SDL_Window* sdlWindow);

    void destroy(bool throwException);


    class Implementations {
    public:

        Implementations() {
            assignReferences<RendererLateBind>();
            assignReferences<RendererVK13>();
        }

        ~Implementations() {
            clearReferences<RendererLateBind>();
            clearReferences<RendererVK13>();
        }

        Implementations(const Implementations&) = delete;
        Implementations& operator=(const Implementations&) = delete;

    private:

        template<Renderer R>
        void assignReferences();

        template<Renderer R>
        void clearReferences();

        VK13Buffer m_bufferImpl;
        VK13Capabilities m_capabilitiesImpl;
        VK13Framebuffer m_mainFramebufferImpl;
        VK13Ordering m_orderingImpl;
        VK13ShaderProgram m_shaderProgramImpl;
        VK13Texture m_textureImpl;
        VK13VertexArray m_vertexArrayImpl;
        VK13Viewport m_viewportImpl;

        ViewportState m_viewportState;
    };

    static VK13Fixture s_fixture;

    Initialized m_init = Initialized::NOTHING;
    vk::Instance m_instance;
    vk::Device m_device;
    vk::CommandPool m_commandPool;
    vk::CommandBuffer m_commandBuffer;
    vk::SurfaceKHR m_surface;

    Implementations m_impls;
};

}