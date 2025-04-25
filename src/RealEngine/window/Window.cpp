/**
 *  @author    Dubsky Tomas
 */
#include <iostream>

#include <ImGui/imgui_impl_sdl2.h>
#include <SDL_events.h>

#include <RealEngine/utility/Error.hpp>
#include <RealEngine/window/Window.hpp>

namespace re {

Window::Window(
    const WindowSettings& settings, const std::string& title,
    const VulkanInitInfo& vulkan, const HotReloadInitInfo& hotReload
)
    : WindowSettings(settings)
    , m_subsystems()
    , m_windowTitle(title)
    , m_usedRenderer(RendererID::Any) {

    m_subsystems.printRealEngineVersion();
    m_subsystems.printSubsystemsVersions();

    initForRenderer(settings.preferredRenderer(), vulkan, hotReload);

    // If the preferred renderer could not be initialized
    if (m_usedRenderer == RendererID::Any) {
        for (size_t i = 0; i < static_cast<size_t>(RendererID::Any);
             i++) { // Try to init any other
            initForRenderer(static_cast<RendererID>(i), vulkan, hotReload);
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
    case RendererID::Vulkan13: m_vk13.~VulkanRenderer(); break;
    case RendererID::Any:      break;
    }
}

void Window::startStep() {
    switch (m_usedRenderer) {
    case RendererID::Vulkan13:
        m_vk13.deletionQueue().startNextIteration(DeletionQueue::Timeline::Step);
        break;
    case RendererID::Any: break;
    }
}

void Window::setMainRenderPass(const RenderPass& rp, uint32_t imGuiSubpassIndex) {
    m_vk13.setMainRenderPass(rp, imGuiSubpassIndex);
}

const CommandBuffer& Window::prepareNewFrame() {
    return m_vk13.prepareFrame();
}

void Window::mainRenderPassBegin(std::span<const vk::ClearValue> clearValues) {
    m_vk13.mainRenderPassBegin(clearValues);
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
    if (m_usedRenderer == RendererID::Vulkan13) {
        m_vk13.changePresentation(m_flags.vSync);
    }
    if (save)
        this->save();
}

std::vector<std::string> Window::availableDevices() const {
    if (m_usedRenderer == RendererID::Vulkan13) {
        return m_vk13.availableDevices();
    }
    return {};
}

void Window::setPreferredDevice(std::string_view preferredDevice, bool save) {
    m_preferredDevice = preferredDevice;
    // Requires restart...
    if (save)
        this->save();
}

std::string Window::usedDevice() const {
    if (m_usedRenderer == RendererID::Vulkan13) {
        return m_vk13.usedDevice();
    }
    return {};
}

void Window::setTitle(const std::string& title) {
    m_windowTitle = title;
    SDL_SetWindowTitle(sdlWindow(), title.c_str());
}

void Window::setPreferredRenderer(RendererID renderer, bool save) {
    m_preferredRenderer = renderer;
    if (save)
        this->save();
}

void Window::setDims(glm::ivec2 newDims, bool save) {
    SDL_SetWindowSize(sdlWindow(), newDims.x, newDims.y);
    SDL_SetWindowPosition(sdlWindow(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_GetWindowSize(sdlWindow(), &m_dims.x, &m_dims.y);
    if (save)
        this->save();
}

void Window::initForRenderer(
    RendererID renderer, const VulkanInitInfo& vulkan, const HotReloadInitInfo& hotReload
) {
    switch (renderer) {
    case RendererID::Vulkan13: initForVulkan13(vulkan, hotReload); break;
    default:                   break;
    }
}

void Window::initForVulkan13(
    const VulkanInitInfo& vulkan, const HotReloadInitInfo& hotReload
) {
    // Create SDL window
    SDL_WindowRAII window = createSDLWindow(RendererID::Vulkan13);
    if (!window) {
        return;
    }

    // Set up Vulkan 1.3 renderer
    try {
        new (&m_vk13) VulkanRenderer{
            window.get(), (bool)m_flags.vSync, m_preferredDevice, vulkan, hotReload
        };
    } catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return;
    }

    // Success
    m_SDLwindow    = std::move(window);
    m_usedRenderer = RendererID::Vulkan13;
}

Window::SDL_WindowRAII Window::createSDLWindow(RendererID renderer) {
    // Prepare window flags
    Uint32 SDL_flags = 0;
    switch (renderer) {
    case RendererID::Vulkan13: SDL_flags |= SDL_WINDOW_VULKAN; break;
    case RendererID::Any:      break;
    }
    if (m_flags.invisible)
        SDL_flags |= SDL_WINDOW_HIDDEN;
    if (m_flags.fullscreen)
        SDL_flags |= SDL_WINDOW_FULLSCREEN;
    if (m_flags.borderless)
        SDL_flags |= SDL_WINDOW_BORDERLESS;

    // Create the window
    SDL_WindowRAII window{SDL_CreateWindow(
        m_windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        m_dims.x, m_dims.y, SDL_flags
    )};

    if (!window) {
        error(SDL_GetError());
    }

    return window;
}

} // namespace re
