/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/main/room/RoomManager.hpp>

#include <RealEngine/main/room/Room.hpp>


namespace RE {

RoomManager::RoomManager() {

}

RoomManager::~RoomManager() {

}

Room* RoomManager::getCurrentRoom() const {
	return m_currentRoom;
}

Room* RoomManager::gotoRoom(size_t index, const RoomTransitionParameters& params) {
	if (index < m_rooms.size()) {//If index is valid
		if (m_currentRoom) {
			m_currentRoom->sessionEnd();//End session of the current room
		}
		m_currentRoom = m_rooms[index];//Get the new room
		m_currentRoom->sessionStart(params);//And start its session
	}
	return m_currentRoom;
}

size_t RoomManager::addRoom(Room* room){
	m_rooms.emplace_back(room);
	return m_rooms.size() - 1;
}

void RoomManager::notifyWindowResized(const glm::ivec2& newSize) {
	for (auto& room : m_rooms) {
		room->windowResized(newSize);
	}
}

}