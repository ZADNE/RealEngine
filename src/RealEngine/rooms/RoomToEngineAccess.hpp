/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/program/Synchronizer.hpp>
#include <RealEngine/rooms/RoomManager.hpp>
#include <RealEngine/user_input/InputManager.hpp>
#include <RealEngine/window/Window.hpp>

namespace re {

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
     * @brief This object is created by the MainProgram and accessed via
     * Room::engine()
     */
    RoomToEngineAccess(
        MainProgram& mainProgram, InputManager& inputManager,
        Synchronizer& synchronizer, Window& window, RoomManager& roomManager
    )
        : m_mainProgram(mainProgram)
        , m_inputManager(inputManager)
        , m_synchronizer(synchronizer)
        , m_window(window)
        , m_roomManager(roomManager) {}

#pragma region MainProgram

    /**
     * @copydoc MainProgram::scheduleExit
     */
    void scheduleExit(int exitcode = EXIT_SUCCESS);

    /**
     * @copydoc MainProgram::scheduleRoomTransition
     */
    void scheduleRoomTransition(size_t name, const RoomTransitionArguments& args);

    /**
     * @copydoc MainProgram::setRelativeCursorMode
     */
    void setRelativeCursorMode(bool relative);

#pragma endregion

#pragma region InputManager

    /**
     * @copydoc InputManager::isDown
     */
    int isKeyDown(Key key) const;

    /**
     * @copydoc InputManager::wasPressed
     */
    int wasKeyPressed(Key key) const;

    /**
     * @copydoc InputManager::wasReleased
     */
    int wasKeyReleased(Key key) const;

    /**
     * @copydoc InputManager::cursorAbs
     */
    glm::ivec2 cursorAbs() const;

    /**
     * @copydoc InputManager::cursorRel
     */
    glm::ivec2 cursorRel() const;

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
     * @copydoc Synchronizer::framesPerSecond
     */
    unsigned int framesPerSecond() const;

    /**
     * @copydoc Synchronizer::maxFrameTime
     */
    Synchronizer::Duration maxFrameTime() const;

#pragma endregion

#pragma region Window

    /**
     * @copydoc Window::flags
     */
    WindowFlags windowFlags() const;

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
     * @copydoc Window::title
     */
    const std::string& windowTitle() const;

    /**
     * @copydoc Window::setDims
     */
    void setWindowDims(glm::ivec2 newDims, bool save);

    /**
     * @copydoc Window::dims
     */
    glm::ivec2 windowDims() const;

    /**
     * @copydoc Window::setPreferredRenderer
     */
    void setPreferredRenderer(RendererID renderer, bool save);

    /**
     * @copydoc Window::preferredRenderer
     */
    RendererID preferredRenderer() const;

    /**
     * @copydoc Window::usedRenderer
     */
    RendererID usedRenderer() const;

    /**
     * @copydoc WindowSettings::save
     */
    void saveWindowSettings();

#pragma endregion

#pragma region Main RenderPass

    constexpr static vk::ClearValue k_defaultClearColor =
        vk::ClearColorValue{1.0f, 1.0f, 1.0f, 1.0f};

    /**
     * @copydoc Window::mainRenderPassBegin()
     */
    void mainRenderPassBegin(
        std::span<const vk::ClearValue> clearValues = {&k_defaultClearColor, 1}
    );

    /**
     * @copydoc Window::mainRenderPassNextSubpass()
     */
    void mainRenderPassNextSubpass();

    /**
     * @copydoc Window::mainRenderPassDrawImGui()
     */
    void mainRenderPassDrawImGui();

    /**
     * @copydoc Window::mainRenderPassEnd()
     */
    void mainRenderPassEnd();

#pragma endregion

private:
    MainProgram& m_mainProgram;
    InputManager& m_inputManager;
    Synchronizer& m_synchronizer;
    Window& m_window;
    RoomManager& m_roomManager;
};

} // namespace re
