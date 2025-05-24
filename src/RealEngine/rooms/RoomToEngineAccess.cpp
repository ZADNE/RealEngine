/**
 *  @author    Dubsky Tomas
 */
#include <RealEngine/program/MainProgram.hpp>
#include <RealEngine/rooms/Room.hpp>
#include <RealEngine/rooms/RoomToEngineAccess.hpp>

namespace re {

#pragma region MainProgram

void RoomToEngineAccess::scheduleExit(int exitcode) {
    m_mainProgram.scheduleExit(exitcode);
}

void RoomToEngineAccess::scheduleRoomTransition(
    size_t name, const RoomTransitionArguments& args
) {
    m_mainProgram.scheduleRoomTransition(name, args);
}

void RoomToEngineAccess::setRelativeCursorMode(bool relative) {
    m_mainProgram.setRelativeCursorMode(relative);
}

#pragma endregion

#pragma region InputManager

int RoomToEngineAccess::isKeyDown(Key key) const {
    return m_inputManager.isDown(key);
}

int RoomToEngineAccess::wasKeyPressed(Key key) const {
    return m_inputManager.wasPressed(key);
}

int RoomToEngineAccess::wasKeyReleased(Key key) const {
    return m_inputManager.wasReleased(key);
}

glm::ivec2 RoomToEngineAccess::cursorAbs() const {
    return m_inputManager.cursorAbs();
}

glm::ivec2 RoomToEngineAccess::cursorRel() const {
    return m_inputManager.cursorRel();
}

#pragma endregion

#pragma region Synchronizer

void RoomToEngineAccess::pauseSteps() {
    m_synchronizer.pauseSteps();
}

void RoomToEngineAccess::resumeSteps() {
    m_synchronizer.resumeSteps();
}

unsigned int RoomToEngineAccess::framesPerSecond() const {
    return m_synchronizer.framesPerSecond();
}

Synchronizer::Duration RoomToEngineAccess::maxFrameTime() const {
    return m_synchronizer.maxFrameTime();
}

#pragma endregion

#pragma region Window

WindowFlags RoomToEngineAccess::windowFlags() const {
    return m_window.flags();
}

void RoomToEngineAccess::setWindowFullscreen(bool fullscreen, bool save) {
    auto prev = m_window.flags();
    m_window.setFullscreen(fullscreen, save);
    auto curr = m_window.flags();
    m_roomManager.notifyRooms<&Room::windowFlagsChangedCallback>(prev, curr);
}

bool RoomToEngineAccess::isWindowFullscreen() const {
    return m_window.isFullscreen();
}

void RoomToEngineAccess::setWindowBorderless(bool borderless, bool save) {
    auto prev = m_window.flags();
    m_window.setBorderless(borderless, save);
    auto curr = m_window.flags();
    m_roomManager.notifyRooms<&Room::windowFlagsChangedCallback>(prev, curr);
}

bool RoomToEngineAccess::isWindowBorderless() const {
    return m_window.isBorderless();
}

void RoomToEngineAccess::setWindowVSync(bool vSync, bool save) {
    auto prev = m_window.flags();
    m_window.setVSync(vSync, save);
    m_renderer.changePresentation(vSync);
    auto curr = m_window.flags();
    m_roomManager.notifyRooms<&Room::windowFlagsChangedCallback>(prev, curr);
}

bool RoomToEngineAccess::isWindowVSynced() const {
    return m_window.isVSynced();
}

std::string_view RoomToEngineAccess::preferredDevice() const {
    return m_window.preferredDevice();
}

std::vector<std::string> RoomToEngineAccess::availableDevices() const {
    return m_renderer.availableDevices();
}

void RoomToEngineAccess::setPreferredDevice(std::string_view preferredDevice, bool save) {
    m_window.setPreferredDevice(preferredDevice, save);
}

std::string RoomToEngineAccess::usedDevice() const {
    return m_renderer.usedDevice();
}

void RoomToEngineAccess::setWindowTitle(const std::string& title) {
    std::string prev = m_window.title();
    m_window.setTitle(title);
    m_roomManager.notifyRooms<&Room::windowTitleChangedCallback>(prev, title);
}

const std::string& RoomToEngineAccess::windowTitle() const {
    return m_window.title();
}

void RoomToEngineAccess::setWindowDims(glm::ivec2 newDims, bool save) {
    glm::ivec2 oldDims = m_window.dims();
    m_window.setDims(newDims, save);
    m_roomManager.notifyRooms<&Room::windowResizedCallback>(oldDims, newDims);
}

glm::ivec2 RoomToEngineAccess::windowDims() const {
    return m_window.dims();
}

void RoomToEngineAccess::saveWindowSettings() {
    m_window.save();
}

#pragma endregion

#pragma region Main RenderPass

void RoomToEngineAccess::mainRenderPassBegin(
    std::span<const vk::ClearValue> clearValues /* = {&k_defaultClearColor, 1}*/
) {
    m_renderer.mainRenderPassBegin(clearValues);
}

void RoomToEngineAccess::mainRenderPassNextSubpass() {
    m_renderer.mainRenderPassNextSubpass();
}

void RoomToEngineAccess::mainRenderPassDrawImGui() {
    m_renderer.mainRenderPassDrawImGui();
}

void RoomToEngineAccess::mainRenderPassEnd() {
    m_renderer.mainRenderPassEnd();
}

#pragma endregion

} // namespace re
