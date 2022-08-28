/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/main/rooms/Room.hpp>

#include <RealEngine/main/program/MainProgram.hpp>

namespace RE {

Room::Room(size_t name, RoomDisplaySettings initialSettings/* = RoomDisplaySettings{}*/) :
	m_displaySettings(initialSettings), m_name(name) {
}

RoomDisplaySettings Room::getDisplaySettings() const {
	return m_displaySettings;
}

void Room::setDisplaySettings(RoomDisplaySettings displaySettings) {
	m_displaySettings = displaySettings;
	if (s_roomManager->getCurrentRoom() == this) {//If this room is active
		s_mainProgram->adoptRoomDisplaySettings(displaySettings);//Notify MainProgram
	}
}

size_t Room::getName() const {
	return m_name;
}

void Room::setStaticReferences(MainProgram* mainProgram, RoomManager* roomManager) {
	s_mainProgram = mainProgram;
	s_roomManager = roomManager;
}

}