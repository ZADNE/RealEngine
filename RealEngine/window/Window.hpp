/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <variant>
#include <string>

#include <ImGui/imgui.h>
#include <glm/vec2.hpp>

#include <RealEngine/window/WindowSubsystems.hpp>
#include <RealEngine/window/WindowSettings.hpp>
#include <RealEngine/window/GL46Fixture.hpp>
#include <RealEngine/window/VK13Fixture.hpp>

union SDL_Event;

namespace RE {

class MainProgram;

/**
 * @brief Displays the scene.
 *
 * There can only be a single window within program
 * and it is created by the MainProgram.
 *
 * The window initializes all subsystems of the RealEngine (SDL2, renderer, ImGui, ...).
*/
class Window : public WindowSettings {
public:

    /**
     * @brief Constructs the window and displays it immediately.
     *
     * @param settings Settings to initialize the window with.
     * @param title Title for the window
    */
    Window(const WindowSettings& settings, const std::string& title);

    /**
     * @brief Destroys the window
    */
    ~Window();

    Window(const Window& other) = delete;
    void operator=(const Window& other) = delete;

    /**
     * @brief Prepares ImGui for new frame
    */
    template<Renderer R>
    void prepareNewFrame();

    /**
     * @brief Draws ImGui
    */
    template<Renderer R>
    void finishNewFrame();

    /**
     * @brief Passes SDL event to ImGui
    */
    void passSDLEvent(SDL_Event& evnt);

    /**
     * @brief Enables/disables ImGui
    */
    void useImGui(bool use) { m_usingImGui = use; }

    /**
     * @brief Checks whether ImGui is used
    */
    bool isImGuiUsed() { return m_usingImGui; }

    /**
     * @brief Switches fullscreen on and off.
     * @param fullscreen True if the window should be fullscreen, false otherwise.
     * @param save Changed settings are saved to file if true.
    */
    void setFullscreen(bool fullscreen, bool save);

    /**
     * @brief Disables and enables window decoration.
     * @param borderless True if the window should have no decoration, false otherwise.
     * @param save Changed settings are saved to file if true.
    */
    void setBorderless(bool borderless, bool save);

    /**
     * @brief Sets usage of vertical synchronization.
     * @param vSync True if vertical synchronization should be used, false for immediate buffer swap.
     * @param save Changed settings are saved to file if true.
    */
    void setVSync(bool vSync, bool save);

    /**
     * @brief Sets new title for the window
     * @param title The new title
    */
    void setTitle(const std::string& title);

    /**
     * @brief Gets current title of the window
    */
    const std::string& getTitle() const;

    /**
     * @brief Resizes the window.
     * @param newDims New dimensions of the window
     * @param save Changed settings are saved to file if true.
    */
    void setDims(const glm::ivec2& newDims, bool save);

    /**
     * @brief Gets current dimensions of the window
    */
    glm::ivec2 getDims() const { return m_dims; }

    /**
     * @brief Gets the used renderer (this can be different from the requested one)
    */
    RendererID getRenderer() const { return m_renderer; }

private:

    void initForRenderer(RendererID renderer);
    void initForVulkan13();
    void initForGL46();

    bool createSDLWindow(RendererID renderer);

    WindowSubsystems m_subsystems;          /**< Empty class that initializes and de-initializes subsystems */
    SDL_Window* m_SDLwindow = nullptr;

    std::variant<std::monostate, VK13Fixture, GL46Fixture>  m_fixture;
    RendererID m_renderer;                  /**< The actual renderer (may be different from the preferred one) */
    std::string m_windowTitle;              /**< Title of the window */
    bool m_usingImGui = false;
};

}
