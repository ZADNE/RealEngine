#pragma once
#include <string>

#include <SDL2/SDL_video.h>
#include <glm/vec2.hpp>

#include <RealEngine/main/details/WindowSettings.hpp>
#include <RealEngine/resources/ResourceManager.hpp>

namespace RE {

class MainProgram;

/**
 * @brief Window that displays the scene.
 *
 * Every window has its own OpenGL context.
*/
class Window : public WindowSettings {
	friend class MainProgram;
public:
	Window(const Window& other) = delete;
	void operator=(const Window& other) = delete;

	/**
	 * @brief Resizes the window.
	 * @param newDims New dimensions of the window
	 * @param save Changed settings are saved to file if true.
	*/
	void resize(const glm::ivec2& newDims, bool save);

	/**
	 * @brief Switches fullscreen on and off.
	 * @param fullscreen True if the window should be fullscreen, false otherwise.
	 * @param save Changed settings are saved to file if true.
	*/
	void goFullscreen(bool fullscreen, bool save);

	/**
	 * @brief Disables and enables window decoration.
	 * @param borderless True if the window should have no decoration, false otherwise.
	 * @param save Changed settings are saved to file if true.
	*/
	void goBorderless(bool borderless, bool save);

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
	 * @return Current title
	*/
	const std::string& getTitle() const;


	/**
	 * @brief Gets current dimensions of the window
	 * @return Current dimensions
	*/
	glm::ivec2 getDims() const { return p_dims; }
private:
	/**
	 * @brief Constructs window.
	 * The window is constructed with its own OpenGL context and is dispalyed immediately.
	 *
	 * @param settings Settings to initialize the window with.
	 * @param title Title for the window
	*/
	Window(const WindowSettings& settings, const std::string& title);

	/**
	 * @brief Destroys the window and its OpenGL context.
	*/
	~Window();

	/**
	 * @brief Swaps buffers if using double buffered context (should be).
	 * This is called after each frame by the main program.
	*/
	void swapBuffer();

	SDL_Window* m_SDLwindow = nullptr;	/**< handle to SDL window */
	SDL_GLContext m_GLContext = nullptr;/**< handle to OpenGL context */
	std::string m_windowTitle;			/**< Title of the window */

	ShaderProgramPtr m_stdSpriteShader;	/**< window also own standard sprite shader */
	ShaderProgramPtr m_stdGeometryShader;/**< window also own standard geometry shader */
};

}
