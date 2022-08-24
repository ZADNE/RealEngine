/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/main/rooms/RoomSystemAccess.hpp>

#include <RealEngine/main/program/MainProgram.hpp>
#include <RealEngine/main/rooms/Room.hpp>

namespace RE {

#pragma region Synchronizer

void RoomSystemAccess::pauseSteps() {
	m_synchronizer.pauseSteps();
}

void RoomSystemAccess::resumeSteps() {
	m_synchronizer.resumeSteps();
}

unsigned int RoomSystemAccess::getFramesPerSecond() const {
	return m_synchronizer.getFramesPerSecond();
}

Synchronizer::Duration RoomSystemAccess::getMaxFrameTime() const {
	return m_synchronizer.getMaxFrameTime();
}

#pragma endregion

#pragma region Window

void RoomSystemAccess::goFullscreen(bool fullscreen, bool save) {
	m_window.goFullscreen(fullscreen, save);
}

bool RoomSystemAccess::isFullscreen() const {
	return m_window.isFullscreen();
}

void RoomSystemAccess::goBorderless(bool borderless, bool save) {
	m_window.goBorderless(borderless, save);
}

bool RoomSystemAccess::isBorderless() const {
	return m_window.isBorderless();
}

void RoomSystemAccess::setVSync(bool vSync, bool save) {
	m_window.setVSync(vSync, save);
}

bool RoomSystemAccess::isVSynced() const {
	return m_window.isVSynced();
}

void RoomSystemAccess::setWindowTitle(const std::string& title) {
	m_window.setTitle(title);
}

const std::string& RoomSystemAccess::getWindowTitle() const {
	return m_window.getTitle();
}

void RoomSystemAccess::setWindowDims(const glm::ivec2& newDims, bool save) {
	auto oldDims = m_window.getDims();
	m_window.setDims(newDims, save);
	m_roomManager.notifyRooms<&Room::windowResizedCallback>(oldDims, newDims);
}

glm::ivec2 RoomSystemAccess::getWindowDims() const {
	return m_window.getDims();
}

void RoomSystemAccess::saveWindowSettings() {
	m_window.save();
}

#pragma endregion

#pragma region MainProgram

void RoomSystemAccess::scheduleExit(int exitcode) {
	m_mainProgram.scheduleExit(exitcode);
}

void RoomSystemAccess::scheduleRoomTransition(size_t name, RoomTransitionParameters params) {
	m_mainProgram.scheduleRoomTransition(name, params);
}

void RoomSystemAccess::setRelativeCursorMode(bool relative) {
	m_mainProgram.setRelativeCursorMode(relative);
}

#pragma endregion


}