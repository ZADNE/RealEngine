/*!
 *  @author    Dubsky Tomas
 */
#pragma once

#include <string>

#include <glm/vec2.hpp>

#include <RealEngine/window/WindowSubsystems.hpp>

namespace re {

/**
 * @brief Specify ancillary parameters of a window
 */
struct WindowFlags {
    WindowFlags()
        : invisible(false)
        , fullscreen(false)
        , borderless(false)
        , vSync(false) {}

    unsigned char invisible : 1, fullscreen : 1, borderless : 1, vSync : 1;
};

/**
 * @brief Specify dimensions and flags of a window.
 * These settings are typically saved so that the settings can be restored
 * on the next startup of the program.
 */
class WindowSettings {
public:
    /**
     * @brief Constructs settings from file that was saved before.
     *
     * If the file cannot be loaded, settings are constructed with most basic options.
     */
    WindowSettings();

    /**
     * @brief Constructs settings from given parameters
     */
    WindowSettings(
        glm::ivec2 dims, WindowFlags flags, RendererID preferredRenderer,
        std::string_view preferredDevice
    );

    /**
     * @brief Gets a copy of the window flags
     */
    WindowFlags flags() const { return m_flags; }

    bool isFullscreen() const { return m_flags.fullscreen; }
    bool isBorderless() const { return m_flags.borderless; }
    bool isVSynced() const { return m_flags.vSync; }

    RendererID preferredRenderer() const { return m_preferredRenderer; }

    std::string_view preferredDevice() const { return m_preferredDevice; }

    /**
     * @brief Save current settings to a file.
     *
     * Saved settings can be loaded by default contructor.
     */
    void save();

protected:
    /**
     * @brief Resets settings to default state.
     */
    void reset();

    glm::ivec2 m_dims;              /**< dimensions of the window */
    WindowFlags m_flags;            /**< flags of the window */
    RendererID m_preferredRenderer; /**< The actual renderer may be different */
    std::string m_preferredDevice;  /**< The actual device may be different */
};

} // namespace re
