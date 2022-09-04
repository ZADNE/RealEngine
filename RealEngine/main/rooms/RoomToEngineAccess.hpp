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
 * @details This can be used to read/modify many parameters and
 * variables of the RealEngine
 * @see Room::engine()
*/
class RoomToEngineAccess {
public:

    /**
     * @brief This object is created by the MainProgram and accessed via Room::engine()
    */
    RoomToEngineAccess(MainProgram& mainProgram, InputManager& inputManager, Synchronizer& synchronizer, Window& window, RoomManager& roomManager) :
        m_mainProgram(mainProgram), m_inputManager(inputManager), m_synchronizer(synchronizer), m_window(window), m_roomManager(roomManager) {}

#pragma region MainProgram

    /**
     * @copydoc MainProgram::scheduleExit
    */
    void scheduleExit(int exitcode = EXIT_SUCCESS);

    /**
     * @copydoc MainProgram::scheduleRoomTransition
    */
    void scheduleRoomTransition(size_t name, const RoomTransitionParameters& params);

    /**
     * @copydoc MainProgram::setRelativeCursorMode
    */
    void setRelativeCursorMode(bool relative);

#pragma endregion

#pragma region InputManager

    /**
     * @copydoc InputManager::isDown
    */
    int isKeyDown(RE::Key keyID) const;

    /**
     * @copydoc InputManager::wasPressed
    */
    int wasKeyPressed(RE::Key keyID) const;

    /**
     * @copydoc InputManager::wasReleased
    */
    int wasKeyReleased(RE::Key keyID) const;

    /**
     * @copydoc InputManager::getCursorAbs
    */
    glm::ivec2 getCursorAbs() const;

    /**
     * @copydoc InputManager::getCursorRel
    */
    glm::ivec2 getCursorRel() const;

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
     * @copydoc Window::getFlags 
    */
    WindowFlags getWindowFlags() const;

    /**
     * @copydoc Window::setFullscreen
    */
    void setWindowFullscreen(bool fullscreen, bool save);

    /**
     * @copydoc Window::isFullscreen
    */
    bool isWindowFullscreen() const; 

    /**
     * @copydoc Window::setBorderless
    */
    void setWindowBorderless(bool borderless, bool save);

    /**
     * @copydoc Window::isBorderless
    */
    bool isWindowBorderless() const;

    /**
     * @copydoc Window::setVSync
    */
    void setWindowVSync(bool vSync, bool save);

    /**
     * @copydoc Window::isVSynced
    */
    bool isWindowVSynced() const;

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