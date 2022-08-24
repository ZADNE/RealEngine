/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/main/rooms/RoomManager.hpp>

#include <RealEngine/main/rooms/Room.hpp>


namespace RE {

RoomManager::RoomManager() {

}

Room* RoomManager::getCurrentRoom() const {
	return m_currentRoom;
}

Room* RoomManager::goToRoom(size_t name, const RoomTransitionParameters& params) {
	auto it = m_rooms.find(name);
	if (it != m_rooms.end()) {//If name is valid
		if (m_currentRoom) {
			m_currentRoom->sessionEnd();//End session of the current room
		}
		m_currentRoom = it->second;//Get the new room
		m_currentRoom->sessionStart(params);//And start its session
	}
	return m_currentRoom;
}

void RoomManager::addRoom(Room* room){
	auto it = m_rooms.find(room->getName());
	if (it != m_rooms.end()) throw std::exception("Room name is not unique!");
	m_rooms.emplace_hint(it, room->getName(), room);
}

}