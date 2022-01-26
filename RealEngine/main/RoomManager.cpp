#include <RealEngine/main/RoomManager.hpp>


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
			p_currentRoom->sessionEnd();//End session of current room
		}
		p_currentRoom = p_rooms[index];//Get the new room
		p_currentRoom->sessionStart(params);//And start its session
	}
	return p_currentRoom;
}

}