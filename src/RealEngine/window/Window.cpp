/*!
 *  @author    Dubsky Tomas
 */
#include <iostream>

#include <ImGui/imgui_impl_sdl.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>

#include <RealEngine/utility/Error.hpp>
#include <RealEngine/window/Window.hpp>

namespace re {

Window::Window(
    const WindowSettings& settings, const std::string& title, const VulkanInitInfo& initInfo
)
    : WindowSettings(settings)
    , m_subsystems()
    , m_windowTitle(title)
    , m_usedRenderer(RendererID::Any) {

    m_subsystems.printRealEngineVersion();
    m_subsystems.printSubsystemsVersions();

    initForRenderer(settings.preferredRenderer(), initInfo);

    // If the preferred renderer could not be initialized
    if (m_usedRenderer == RendererID::Any) {
        for (size_t i = 0; i < static_cast<size_t>(RendererID::Any);
             i++) { // Try to init any other
            initForRenderer(static_cast<RendererID>(i), initInfo);
            if (m_usedRenderer != RendererID::Any)
                break;
        }

        // If no renderer could be initialized
        if (m_usedRenderer == RendererID::Any) {
            // There is nothing more we can do
            fatalError("No renderer could be initialized!");
        }
    }
}

Window::~Window() {
    switch (m_usedRenderer) {
    case RendererID::Vulkan13: m_vk13.~VulkanFixture(); break;
    }
    SDL_DestroyWindow(m_SDLwindow);
}

const re::CommandBuffer& Window::prepareNewFrame() {
    return m_vk13.prepareFrame(m_usingImGui);
}

void Window::mainRenderPassBegin() {
    m_vk13.mainRenderPassBegin(m_clearValues);
}

void Window::mainRenderPassNextSubpass() {
    m_vk13.mainRenderPassNextSubpass();
}

void Window::mainRenderPassDrawImGui() {
    m_vk13.mainRenderPassDrawImGui();
}

void Window::mainRenderPassEnd() {
    m_vk13.mainRenderPassEnd();
}

void Window::finishNewFrame() {
    m_vk13.finishFrame();
}

void Window::prepareForDestructionOfRendererObjects() {
    m_vk13.prepareForDestructionOfRendererObjects();
}

bool Window::passSDLEvent(const SDL_Event& evnt) {
    ImGui_ImplSDL2_ProcessEvent(&evnt);
    auto& io = ImGui::GetIO();
    switch (evnt.type) {
    case SDL_KEYUP:
    case SDL_KEYDOWN: return io.WantCaptureKeyboard;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEMOTION:
    case SDL_MOUSEWHEEL: return io.WantCaptureMouse;
    default: return false;
    }
}

void Window::setFullscreen(bool fullscreen, bool save) {
    m_flags.fullscreen = fullscreen;
    SDL_SetWindowFullscreen(m_SDLwindow, (fullscreen) ? SDL_WINDOW_FULLSCREEN : 0);
    SDL_GetWindowSize(m_SDLwindow, &m_dims.x, &m_dims.y);
    if (save)
        this->save();
}

void Window::setBorderless(bool borderless, bool save) {
    m_flags.borderless = borderless;
    SDL_SetWindowBordered(m_SDLwindow, (borderless) ? SDL_FALSE : SDL_TRUE);
    if (save)
        this->save();
}

void Window::setVSync(bool vSync, bool save) {
    m_flags.vSync = vSync;
    if (m_usedRenderer == RendererID::Vulkan13) {
        m_vk13.changePresentation(m_flags.vSync);
    }
    if (save)
        this->save();
}

void Window::setTitle(const std::string& title) {
    m_windowTitle = title;
    SDL_SetWindowTitle(m_SDLwindow, title.c_str());
}

void Window::setPreferredRenderer(RendererID renderer, bool save) {
    m_preferredRenderer = renderer;
    if (save)
        this->save();
}

void Window::setDims(glm::ivec2 newDims, bool save) {
    SDL_SetWindowSize(m_SDLwindow, newDims.x, newDims.y);
    SDL_SetWindowPosition(m_SDLwindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_GetWindowSize(m_SDLwindow, &m_dims.x, &m_dims.y);
    if (save)
        this->save();
}

void Window::initForRenderer(RendererID renderer, const VulkanInitInfo& initInfo) {
    switch (renderer) {
    case RendererID::Vulkan13: initForVulkan13(initInfo); break;
    default: break;
    }
}

void Window::initForVulkan13(const VulkanInitInfo& initInfo) {
    // Create SDL window
    if (!createSDLWindow(RendererID::Vulkan13)) {
        goto fail;
    }

    // Set up Vulkan 1.3 fixture
    try {
        new (&m_vk13) VulkanFixture{m_SDLwindow, (bool)m_flags.vSync, initInfo};
    } catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        goto fail_SDLWindow;
    }

    m_usedRenderer = RendererID::Vulkan13;
    return;

fail_SDLWindow:
    SDL_DestroyWindow(m_SDLwindow);
    m_SDLwindow = nullptr;
fail:
    m_usedRenderer = RendererID::Any;
}

bool Window::createSDLWindow(RendererID renderer) {
    // Prepare window flags
    Uint32 SDL_flags = 0;
    switch (renderer) {
    case RendererID::Vulkan13: SDL_flags |= SDL_WINDOW_VULKAN; break;
    }
    if (m_flags.invisible)
        SDL_flags |= SDL_WINDOW_HIDDEN;
    if (m_flags.fullscreen)
        SDL_flags |= SDL_WINDOW_FULLSCREEN;
    if (m_flags.borderless)
        SDL_flags |= SDL_WINDOW_BORDERLESS;

    // Create the window
    m_SDLwindow = SDL_CreateWindow(
        m_windowTitle.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        m_dims.x,
        m_dims.y,
        SDL_flags
    );
    if (!m_SDLwindow) {
        log(SDL_GetError());
        return false;
    }

    return true;
}

} // namespace re