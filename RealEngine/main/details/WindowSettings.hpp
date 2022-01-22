#pragma once

#include <string>

#include <glm/vec2.hpp>

namespace RE {

/**
 * @brief Window flags specify ancillary parameters of a window
*/
struct WindowFlags {
	WindowFlags() : invisible(false), fullscreen(false), borderless(false), vSync(false) {}

	unsigned char invisible : 1, fullscreen : 1, borderless : 1, vSync : 1;
};

/**
 * @brief Windows settings specify dimensions and flags of a window.
 * These settings are typically saved so that the settings can be restored
 * on the next startup of the program.
*/
class WindowSettings {
public:
	/**
	 * @brief Constructs settings from file that was saved before.
	 * If the file cannot be loaded, settings are constructed with most basic options.
	*/
	WindowSettings();

	/**
	 * @brief Constructs settings
	 * @param dims Dimensions of window
	 * @param flags Flags of the window
	*/
	WindowSettings(const glm::ivec2& dims, WindowFlags flags);

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

	glm::ivec2 p_dims;	/**< dimensions of the window */
	WindowFlags p_flags;/**< flags of the window */
};

}