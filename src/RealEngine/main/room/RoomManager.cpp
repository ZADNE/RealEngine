/*! 
 *  \author    Dubsky Tomas
 */
#include <RealEngine/main/room/RoomManager.hpp>

#include <RealEngine/main/room/Room.hpp>


namespace RE {

RoomManager::RoomManager() {

}

RoomManager::~RoomManager() {

}

Room* RoomManager::getCurrentRoom() const {
	return p_currentRoom;
}

Room* RoomManager::gotoRoom(size_t index, const RoomTransitionParameters& params) {
	if (index < p_rooms.size()) {//If index is valid
		if (p_currentRoom) {
			p_currentRoom->sessionEnd();//End session of the current room
		}
		p_currentRoom = p_rooms[index];//Get the new room
		p_currentRoom->sessionStart(params);//And start its session
	}
	return p_currentRoom;
}

size_t RoomManager::addRoom(Room* room){
	p_rooms.emplace_back(room);
	return p_rooms.size() - 1;
}

void RoomManager::notifyWindowResized(const glm::ivec2& newSize) {
	for (auto& room : p_rooms) {
		room->windowResized(newSize);
	}
}

}