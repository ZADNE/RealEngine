/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/main/rooms/Room.hpp>

namespace RE {

Room::Room(size_t name, RoomDisplaySettings initialSettings/* = RoomDisplaySettings{}*/) :
	m_displaySettings(initialSettings), m_name(name) {
	s_roomManager->addRoom(this);
}

RoomDisplaySettings Room::getDisplaySettings() const {
	return m_displaySettings;
}

void Room::setDisplaySettings(RoomDisplaySettings displaySettings) {
	m_displaySettings = displaySettings;
}

size_t Room::getName() const {
	return m_name;
}

}