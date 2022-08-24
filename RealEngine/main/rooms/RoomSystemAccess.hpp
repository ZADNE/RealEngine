/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/user_input/InputManager.hpp>
#include <RealEngine/main/program/Synchronizer.hpp>
#include <RealEngine/main/window/Window.hpp>
#include <RealEngine/main/rooms/RoomManager.hpp>

namespace RE {

class MainProgram;

/**
 * @brief Is a proxy-like object to many RealEngine systems
 * @details This class contains everything systems-related that a Room can access.
 * @see Room::system()
*/
class RoomSystemAccess {
public:

	/**
	 * @brief This object is created by the MainProgram and accessed via Room::system()
	*/
	RoomSystemAccess(MainProgram& mainProgram, InputManager& inputManager, Synchronizer& synchronizer, Window& window, RoomManager& roomManager) :
		m_mainProgram(mainProgram), m_inputManager(inputManager), m_synchronizer(synchronizer), m_window(window), m_roomManager(roomManager) {}

#pragma region MainProgram

	/**
	 * @copydoc MainProgram::scheduleExit
	*/
	void scheduleExit(int exitcode = EXIT_SUCCESS);

	/**
	 * @copydoc MainProgram::scheduleRoomTransition
	*/
	void scheduleRoomTransition(size_t name, RoomTransitionParameters params);

	/**
	 * @copydoc MainProgram::setRelativeCursorMode
	*/
	void setRelativeCursorMode(bool relative);

#pragma endregion

#pragma region Synchronizer

	/**
	 * @copydoc Synchronizer::pauseSteps
	*/
	void pauseSteps();

	/**
	 * @copydoc Synchronizer::resumeSteps
	*/
	void resumeSteps();

	/**
	 * @copydoc Synchronizer::getFramesPerSecond
	*/
	unsigned int getFramesPerSecond() const;

	/**
	 * @copydoc Synchronizer::getMaxFrameTime
	*/
	Synchronizer::Duration getMaxFrameTime() const;

#pragma endregion

#pragma region Window

	/**
	 * @copydoc Window::goFullscreen
	*/
	void goFullscreen(bool fullscreen, bool save);

	bool isFullscreen() const; 

	/**
	 * @copydoc Window::goBorderless
	*/
	void goBorderless(bool borderless, bool save);

	bool isBorderless() const;

	/**
	 * @copydoc Window::setVSync
	*/
	void setVSync(bool vSync, bool save);

	bool isVSynced() const;

	/**
	 * @copydoc Window::setTitle
	*/
	void setWindowTitle(const std::string& title);

	/**
	 * @copydoc Window::getTitle
	*/
	const std::string& getWindowTitle() const;

	/**
	 * @brief Window::setDims
	*/
	void setWindowDims(const glm::ivec2& newDims, bool save);

	/**
	 * @copydoc Window::getDims
	*/
	glm::ivec2 getWindowDims() const;

	/**
	 * @copydoc WindowSettings::save
	*/
	void saveWindowSettings();

#pragma endregion

private:

	MainProgram& m_mainProgram;
	InputManager& m_inputManager;
	Synchronizer& m_synchronizer;
	Window& m_window;
	RoomManager& m_roomManager;
};

}