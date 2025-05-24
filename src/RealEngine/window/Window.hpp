/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>

#include <ImGui/imgui.h>
#include <SDL_video.h>
#include <glm/vec2.hpp>

#include <RealEngine/utility/UniqueCPtr.hpp>
#include <RealEngine/window/WindowSettings.hpp>
#include <RealEngine/window/WindowSubsystems.hpp>

union SDL_Event;

namespace re {

class MainProgram;

/**
 * @brief Displays the scene.
 *
 * There can only be a single window within program
 * and it is created by the MainProgram.
 *
 * The window initializes some subsystems of the RealEngine (SDL2, ImGui, ...).
 */
class Window: public WindowSettings {
public:
    /**
     * @brief Constructs the window and displays it immediately.
     *
     * @param settings Settings to initialize the window with.
     * @param title Title for the window
     */
    Window(const WindowSettings& settings, const std::string& title);

    /**
     * @brief Passes SDL event to ImGui
     * @return True if the event has been consumed, false otherwise
     */
    bool passSDLEvent(const SDL_Event& evnt);

    /**
     * @brief Switches fullscreen on and off.
     * @param fullscreen True if the window should be fullscreen, false otherwise.
     * @param save Changed settings are saved to file if true.
     */
    void setFullscreen(bool fullscreen, bool save);

    /**
     * @brief Disables and enables window decoration.
     * @param borderless True if the window should have no decoration, false
     * otherwise.
     * @param save Changed settings are saved to file if true.
     */
    void setBorderless(bool borderless, bool save);

    /**
     * @brief Sets usage of vertical synchronization.
     * @param vSync True if vertical synchronization should be used, false for
     * immediate buffer swap.
     * @param save Changed settings are saved to file if true.
     */
    void setVSync(bool vSync, bool save);

    /**
     * @brief Sets name of the preferred device
     * @param preferredDevice Name of the device must match exactly the name
     *                        reported by underlying driver.
     * @param save Changed settings are saved to file if true.
     * @note The program must be restarted for the action to take effect and the
     *       device must be suitable (support all requested features).
     */
    void setPreferredDevice(std::string_view preferredDevice, bool save);

    /**
     * @brief Sets new title for the window
     * @param title The new title
     */
    void setTitle(const std::string& title);

    /**
     * @brief Gets current title of the window
     */
    const std::string& title() const { return m_windowTitle; }

    /**
     * @brief Resizes the window.
     * @param newDims New dimensions of the window
     * @param save Changed settings are saved to file if true.
     */
    void setDims(glm::ivec2 newDims, bool save);

    /**
     * @brief Gets current dimensions of the window
     */
    glm::ivec2 dims() const { return m_dims; }

    /**
     * @brief Used only to initialize renderer
     */
    SDL_Window* sdlWindow() { return m_SDLwindow.get(); }

private:
    using SDL_WindowRAII = UniqueCPtr<SDL_Window, SDL_DestroyWindow>;
    SDL_WindowRAII createSDLWindow();

    WindowSubsystems m_subsystems; ///< Initializes and de-initializes subsystems
    SDL_WindowRAII m_SDLwindow;

    std::string m_windowTitle;
};

} // namespace re
