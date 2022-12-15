/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/window/Window.hpp>

#include <iostream>

#include <SDL2/SDL_video.h>
#include <ImGui/imgui_impl_sdl.h>

#include <RealEngine/utility/Error.hpp>
#include <RealEngine/rendering/output/Viewport.hpp>


namespace RE {

Window::Window(const WindowSettings& settings, const std::string& title) :
    WindowSettings(settings), m_subsystems(), m_windowTitle(title), m_usedRenderer(RendererID::ANY) {

    m_subsystems.printRealEngineVersion();
    m_subsystems.printSubsystemsVersions();

    initForRenderer(settings.getPreferredRenderer());

    if (m_usedRenderer == RendererID::ANY) {//If the preferred renderer could not be initialized
        for (size_t i = 0; i < static_cast<size_t>(RendererID::ANY); i++) {//Try to init any other
            initForRenderer(static_cast<RendererID>(i));
            if (m_usedRenderer != RendererID::ANY) break;
        }

        if (m_usedRenderer == RendererID::ANY) {//If no renderer could be initialized
            //There is nothing more we can do
            fatalError("No renderer could be initialized!");
        }
    }

    Viewport<>::s_state->windowSize = m_dims;
    Viewport<>::s_state->trackingWindow = true;

    assert(m_usedRenderer == RendererID::VULKAN13 || m_usedRenderer == RendererID::OPENGL46);
}

Window::~Window() {
    switch (m_usedRenderer) {
    case RendererID::VULKAN13:
        m_vk13.~VK13Fixture(); break;
    }
    SDL_DestroyWindow(m_SDLwindow);
}

template<>
const vk::CommandBuffer& Window::prepareNewFrame<RendererVK13>() {
    return m_vk13.prepareFrame(m_clearColor, m_usingImGui);
}

template<>
void Window::finishNewFrame<RendererVK13>() {
    m_vk13.finishFrame(m_usingImGui);
}

template<>
void Window::prepareForDestructionOfRendererObjects<RendererVK13>() {
    m_vk13.prepareForDestructionOfRendererObjects();
}

void Window::passSDLEvent(SDL_Event& evnt) {
    ImGui_ImplSDL2_ProcessEvent(&evnt);
}

void Window::setFullscreen(bool fullscreen, bool save) {
    m_flags.fullscreen = fullscreen;
    SDL_SetWindowFullscreen(m_SDLwindow, (fullscreen) ? SDL_WINDOW_FULLSCREEN : 0);
    SDL_GetWindowSize(m_SDLwindow, &m_dims.x, &m_dims.y);
    if (save) this->save();
}

void Window::setBorderless(bool borderless, bool save) {
    m_flags.borderless = borderless;
    SDL_SetWindowBordered(m_SDLwindow, (borderless) ? SDL_FALSE : SDL_TRUE);
    if (save) this->save();
}

void Window::setVSync(bool vSync, bool save) {
    m_flags.vSync = vSync;
    if (m_usedRenderer == RendererID::VULKAN13) {
        m_vk13.changePresentation(m_flags.vSync);
    } else if (m_usedRenderer == RendererID::OPENGL46) {
        if (m_flags.vSync) {
            if (SDL_GL_SetSwapInterval(-1)) {
                //Cannot use adaptive vSync, use regular vSync
                log(SDL_GetError());
                SDL_GL_SetSwapInterval(1);
            }
        } else {
            SDL_GL_SetSwapInterval(0);
        }
    }
    if (save) this->save();
}

void Window::setTitle(const std::string& title) {
    m_windowTitle = title;
    SDL_SetWindowTitle(m_SDLwindow, title.c_str());
}

const std::string& Window::getTitle() const {
    return m_windowTitle;
}

void Window::setPreferredRenderer(RendererID renderer, bool save) {
    m_preferredRenderer = renderer;
    if (save) this->save();
}

void Window::setDims(const glm::ivec2& newDims, bool save) {
    SDL_SetWindowSize(m_SDLwindow, newDims.x, newDims.y);
    SDL_SetWindowPosition(m_SDLwindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_GetWindowSize(m_SDLwindow, &m_dims.x, &m_dims.y);

    Viewport<>::s_state->windowSize = m_dims;
    if (Viewport<>::s_state->trackingWindow) {
        Viewport<>::setToWholeWindow();
    }
    if (save) this->save();
}

void Window::initForRenderer(RendererID renderer) {
    switch (renderer) {
    case RendererID::VULKAN13:      initForVulkan13(); break;
    default:                        break;
    }
}

void Window::initForVulkan13() {
    //Create SDL window
    if (!createSDLWindow(RendererID::VULKAN13)) {
        goto fail;
    }

    //Create OpenGL 4.6 fixture
    try {
        new (&m_vk13) VK13Fixture(m_SDLwindow, (bool)m_flags.vSync);
    }
    catch (std::exception& e) {
        std::cerr << e.what();
        goto fail_SDLWindow;
    }

    m_usedRenderer = RendererID::VULKAN13;
    return;

fail_SDLWindow:
    SDL_DestroyWindow(m_SDLwindow);
    m_SDLwindow = nullptr;
fail:
    m_usedRenderer = RendererID::ANY;
}

bool Window::createSDLWindow(RendererID renderer) {
    //Prepare window flags
    Uint32 SDL_flags = 0;
    switch (renderer) {
    case RendererID::VULKAN13: SDL_flags |= SDL_WINDOW_VULKAN; break;
    }
    if (m_flags.invisible) SDL_flags |= SDL_WINDOW_HIDDEN;
    if (m_flags.fullscreen) SDL_flags |= SDL_WINDOW_FULLSCREEN;
    if (m_flags.borderless) SDL_flags |= SDL_WINDOW_BORDERLESS;

    //Create the window
    m_SDLwindow = SDL_CreateWindow(m_windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_dims.x, m_dims.y, SDL_flags);
    if (!m_SDLwindow) {
        log(SDL_GetError());
        return false;
    }

    return true;
}

}