/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/main/rooms/RoomToEngineAccess.hpp>

#include <RealEngine/main/program/MainProgram.hpp>
#include <RealEngine/main/rooms/Room.hpp>

namespace RE {

#pragma region MainProgram

void RoomToEngineAccess::scheduleExit(int exitcode) {
	m_mainProgram.scheduleExit(exitcode);
}

void RoomToEngineAccess::scheduleRoomTransition(size_t name, RoomTransitionParameters params) {
	m_mainProgram.scheduleRoomTransition(name, params);
}

void RoomToEngineAccess::setRelativeCursorMode(bool relative) {
	m_mainProgram.setRelativeCursorMode(relative);
}

#pragma endregion

#pragma region InputManager

int RoomToEngineAccess::isKeyDown(RE::Key keyID) const {
	return m_inputManager.isDown(keyID);
}

int RoomToEngineAccess::wasKeyPressed(RE::Key keyID) const {
	return m_inputManager.wasPressed(keyID);
}

int RoomToEngineAccess::wasKeyReleased(RE::Key keyID) const {
	return m_inputManager.wasReleased(keyID);
}

glm::ivec2 RoomToEngineAccess::getCursorAbs() const {
	return m_inputManager.getCursorAbs();
}

glm::ivec2 RoomToEngineAccess::getCursorRel() const {
	return m_inputManager.getCursorRel();
}

#pragma endregion

#pragma region Synchronizer

void RoomToEngineAccess::pauseSteps() {
	m_synchronizer.pauseSteps();
}

void RoomToEngineAccess::resumeSteps() {
	m_synchronizer.resumeSteps();
}

unsigned int RoomToEngineAccess::getFramesPerSecond() const {
	return m_synchronizer.getFramesPerSecond();
}

Synchronizer::Duration RoomToEngineAccess::getMaxFrameTime() const {
	return m_synchronizer.getMaxFrameTime();
}

#pragma endregion

#pragma region Window

WindowFlags RoomToEngineAccess::getWindowFlags() const {
	return m_window.getFlags();
}

void RoomToEngineAccess::setWindowFullscreen(bool fullscreen, bool save) {
	auto prev = m_window.getFlags();
	m_window.setFullscreen(fullscreen, save);
	auto curr = m_window.getFlags();
	m_roomManager.notifyRooms<&Room::windowFlagsChangedCallback>(prev, curr);
}

bool RoomToEngineAccess::isWindowFullscreen() const {
	return m_window.isFullscreen();
}

void RoomToEngineAccess::setWindowBorderless(bool borderless, bool save) {
	auto prev = m_window.getFlags();
	m_window.setBorderless(borderless, save);
	auto curr = m_window.getFlags();
	m_roomManager.notifyRooms<&Room::windowFlagsChangedCallback>(prev, curr);
}

bool RoomToEngineAccess::isWindowBorderless() const {
	return m_window.isBorderless();
}

void RoomToEngineAccess::setWindowVSync(bool vSync, bool save) {
	auto prev = m_window.getFlags();
	m_window.setVSync(vSync, save);
	auto curr = m_window.getFlags();
	m_roomManager.notifyRooms<&Room::windowFlagsChangedCallback>(prev, curr);
}

bool RoomToEngineAccess::isWindowVSynced() const {
	return m_window.isVSynced();
}

void RoomToEngineAccess::setWindowTitle(const std::string& title) {
	std::string prev = m_window.getTitle();
	m_window.setTitle(title);
	m_roomManager.notifyRooms<&Room::windowTitleChangedCallback>(prev, title);
}

const std::string& RoomToEngineAccess::getWindowTitle() const {
	return m_window.getTitle();
}

void RoomToEngineAccess::setWindowDims(const glm::ivec2& newDims, bool save) {
	auto oldDims = m_window.getDims();
	m_window.setDims(newDims, save);
	m_roomManager.notifyRooms<&Room::windowResizedCallback>(oldDims, newDims);
}

glm::ivec2 RoomToEngineAccess::getWindowDims() const {
	return m_window.getDims();
}

void RoomToEngineAccess::saveWindowSettings() {
	m_window.save();
}

#pragma endregion

}