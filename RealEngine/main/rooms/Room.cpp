/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/main/rooms/Room.hpp>

namespace RE {

Room::Room(RoomDisplaySettings initialSettings/* = RoomDisplaySettings{}*/):
	m_displaySettings(initialSettings) {

}

void Room::windowResized(const glm::ivec2& newSize) {

}

MainProgram* Room::program() const {
	return s_mainProgram;
}

const InputManager* Room::input() const {
	return s_inputManager;
}

Synchronizer* Room::synchronizer() const {
	return s_synchronizer;
}

Window* Room::window() const {
	return s_window;
}

RoomDisplaySettings Room::getDisplaySettings() const {
	return m_displaySettings;
}

void Room::setDisplaySettings(RoomDisplaySettings displaySettings) {
	m_displaySettings = displaySettings;
}

}