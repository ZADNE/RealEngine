#include <RealEngine/main/RoomManager.hpp>


namespace RE {

RoomManager::RoomManager() {

}

RoomManager::~RoomManager() {

}

Room* RoomManager::getCurrentRoom() const {
	return p_currentRoom;
}

Room* RoomManager::gotoRoom(size_t index) {
	RoomTransitionParameters transitionParams;
	if (isIndexValid(index)) {
		if (p_currentRoom) {
			transitionParams = p_currentRoom->E_exit();
		}
		p_currentRoom = p_rooms[index].get();
		p_currentRoom->E_entry(transitionParams);
	}
	return p_currentRoom;
}

bool RoomManager::isIndexValid(size_t index) const {
	if (index < p_rooms.size()) {
		return true;
	}
	return false;
}

}