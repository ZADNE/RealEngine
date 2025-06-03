/**
 *  @author    Dubsky Tomas
 */
#include <iostream>

#include <ImGui/imgui_impl_sdl2.h>
#include <SDL_events.h>

#include <RealEngine/utility/Error.hpp>
#include <RealEngine/window/Window.hpp>

namespace re {

Window::Window(const WindowSettings& settings, const std::string& title)
    : WindowSettings{settings}
    , m_subsystems{}
    , m_SDLwindow{createSDLWindow()}
    , m_windowTitle{title} {

    m_subsystems.printRealEngineVersion();
    m_subsystems.printSubsystemsVersions();
}

bool Window::passSDLEvent(const SDL_Event& evnt) {
    ImGui_ImplSDL2_ProcessEvent(&evnt);
    auto& io = ImGui::GetIO();
    switch (evnt.type) {
    case SDL_KEYUP:
    case SDL_KEYDOWN:         return io.WantCaptureKeyboard;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEMOTION:
    case SDL_MOUSEWHEEL:      return io.WantCaptureMouse;
    default:                  return false;
    }
}

void Window::setFullscreen(bool fullscreen, bool save) {
    m_flags.fullscreen = fullscreen;
    SDL_SetWindowFullscreen(sdlWindow(), (fullscreen) ? SDL_WINDOW_FULLSCREEN : 0);
    SDL_GetWindowSize(sdlWindow(), &m_dims.x, &m_dims.y);
    if (save)
        this->save();
}

void Window::setBorderless(bool borderless, bool save) {
    m_flags.borderless = borderless;
    SDL_SetWindowBordered(sdlWindow(), (borderless) ? SDL_FALSE : SDL_TRUE);
    if (save)
        this->save();
}

void Window::setVSync(bool vSync, bool save) {
    m_flags.vSync = vSync;
    if (save)
        this->save();
}

void Window::setPreferredDevice(std::string_view preferredDevice, bool save) {
    m_preferredDevice = preferredDevice;
    // Requires restart...
    if (save)
        this->save();
}

void Window::setTitle(const std::string& title) {
    m_windowTitle = title;
    SDL_SetWindowTitle(sdlWindow(), title.c_str());
}

void Window::setDims(glm::ivec2 newDims, bool save) {
    SDL_SetWindowSize(sdlWindow(), newDims.x, newDims.y);
    SDL_SetWindowPosition(sdlWindow(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_GetWindowSize(sdlWindow(), &m_dims.x, &m_dims.y);
    if (save)
        this->save();
}

Window::SDL_WindowRAII Window::createSDLWindow() {
    // Prepare window flags
    Uint32 SDL_flags = SDL_WINDOW_VULKAN;
    if (m_flags.invisible)
        SDL_flags |= SDL_WINDOW_HIDDEN;
    if (m_flags.fullscreen)
        SDL_flags |= SDL_WINDOW_FULLSCREEN;
    if (m_flags.borderless)
        SDL_flags |= SDL_WINDOW_BORDERLESS;

    auto toSDLPos = [](int pos) -> int {
        return pos == k_centeredWindowPosition ? SDL_WINDOWPOS_CENTERED : pos;
    };

    // Create the window
    SDL_WindowRAII window{SDL_CreateWindow(
        m_windowTitle.c_str(), toSDLPos(m_pos.x), toSDLPos(m_pos.y), m_dims.x,
        m_dims.y, SDL_flags
    )};

    if (!window) {
        error(SDL_GetError());
    }

    return window;
}

} // namespace re
