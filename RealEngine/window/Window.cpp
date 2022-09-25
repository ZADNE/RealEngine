#include "Window.hpp"
/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/window/Window.hpp>

#include <SDL2/SDL_vulkan.h>

#include <ImGui/imgui_impl_sdl.h>
#include <ImGui/imgui_impl_vulkan.h>
#include <ImGui/imgui_impl_opengl3.h>

#include <RealEngine/window/GL46Fixture.hpp>
#include <RealEngine/window/VK13Fixture.hpp>
#include <RealEngine/utility/Error.hpp>
#include <RealEngine/rendering/output/Viewport.hpp>


namespace RE {

Window::Window(const WindowSettings& settings, const std::string& title) :
    WindowSettings(settings), m_subsystems(), m_windowTitle(title), m_renderer(RendererID::ANY) {

    m_subsystems.printRealEngineVersion();
    m_subsystems.printSubsystemsVersions();

    initForRenderer(settings.getPreferredRenderer());

    if (m_renderer == RendererID::ANY) {//If the preferred renderer could not be initialized
        for (size_t i = 0; i < static_cast<size_t>(RendererID::ANY); i++) {//Try to init any 
            initForRenderer(static_cast<RendererID>(i));
            if (m_renderer != RendererID::ANY) break;
        }

        if (m_renderer == RendererID::ANY) {//If no renderer could be initialized
            //There is nothing more we can do
            fatalError("No renderer could be initialized!");
        }
    }

    //Set vertical synchronisation
    setVSync(m_flags.vSync, false);

    Viewport<>::s_state->windowSize = m_dims;
    Viewport<>::s_state->trackingWindow = true;

    assert(m_renderer == RendererID::VULKAN13 || m_renderer == RendererID::OPENGL46);
}

Window::~Window() {
    switch (m_renderer) {
    case RE::RendererID::VULKAN13:
        ImGui_ImplVulkan_Shutdown();
        break;
    case RE::RendererID::OPENGL46:
        ImGui_ImplOpenGL3_Shutdown();
        SDL_GL_DeleteContext(m_GLContext);
        break;
    }

    ImGui_ImplSDL2_Shutdown();

    SDL_DestroyWindow(m_SDLwindow);
}

template<>
void Window::prepareNewFrame<RendererVK13>() {
    if (m_usingImGui) {//ImGui frame start
        ImGui::NewFrame();
    }
}

template<>
void Window::prepareNewFrame<RendererGL46>() {
    if (m_usingImGui) {//ImGui frame start
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }
}

template<>
void Window::finishNewFrame<RendererVK13>() {
    if (m_usingImGui) {//ImGui frame end
        ImGui::Render();
    }
}

template<>
void Window::finishNewFrame<RendererGL46>() {
    if (m_usingImGui) {//ImGui frame end
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(m_SDLwindow);
    }
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
    if (m_flags.vSync) {
        if (SDL_GL_SetSwapInterval(-1)) {
            //Cannot use adaptive vSync, use regular vSync
            log(SDL_GetError());
            SDL_GL_SetSwapInterval(1);
        }
    } else {
        SDL_GL_SetSwapInterval(0);
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
    case RendererID::OPENGL46:      initForGL46(); break;
    default:                        break;
    }
}

void Window::initForVulkan13() {
    if (!VK13Fixture::prepare()) goto fail;

    //Create SDL window
    if (!createSDLWindow(RendererID::VULKAN13)) {
        goto fail;
    }

    //Initialize some global states to RealEngine-default values
    VK13Fixture::initialize();

    if (!ImGui_ImplSDL2_InitForVulkan(m_SDLwindow)) goto fail_SDLWindow;
    //if (!ImGui_ImplVulkan_Init("#version 460 core")) goto fail_SDLWindow;

    m_renderer = RendererID::VULKAN13;
    return;

fail_SDLWindow:
    SDL_DestroyWindow(m_SDLwindow);
    m_SDLwindow = nullptr;
fail:
    m_renderer = RendererID::ANY;
}

void Window::initForGL46() {
    //Set OpenGL context creation parameters
    if (!GL46Fixture::prepare()) goto fail;

    //Create SDL window
    if (!createSDLWindow(RendererID::OPENGL46)) {
        goto fail;
    }

    //Create OpenGL context for the window
    m_GLContext = SDL_GL_CreateContext(m_SDLwindow);
    if (!m_GLContext) {
        log(SDL_GetError());
        goto fail_SDLWindow;
    }

    //Initialize some global states to RealEngine-default values
    GL46Fixture::initialize();

    if (!ImGui_ImplSDL2_InitForOpenGL(m_SDLwindow, m_GLContext)) goto fail_GLContext_SDLWindow;
    if (!ImGui_ImplOpenGL3_Init("#version 460 core")) goto fail_GLContext_SDLWindow;

    m_renderer = RendererID::OPENGL46;
    return;

fail_GLContext_SDLWindow:
    SDL_GL_DeleteContext(m_GLContext);
    m_GLContext = nullptr;
fail_SDLWindow:
    SDL_DestroyWindow(m_SDLwindow);
    m_SDLwindow = nullptr;
fail:
    m_renderer = RendererID::ANY;
}

bool Window::createSDLWindow(RendererID renderer) {
    //Prepare window flags
    Uint32 SDL_flags = 0;
    switch (renderer) {
    case RE::RendererID::VULKAN13: SDL_flags |= SDL_WINDOW_VULKAN; break;
    case RE::RendererID::OPENGL46: SDL_flags |= SDL_WINDOW_OPENGL; break;
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