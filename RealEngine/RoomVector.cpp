#include <RealEngine/RoomVector.hpp>
#include <RealEngine/Room.hpp>

namespace RE {

	RoomVector::RoomVector(MainProgram* mainProgram) : p_program(mainProgram) {

	}

	RoomVector::~RoomVector() {
		destroy();
	}

	Room* RoomVector::getCurrentRoom() const {
		if (!isIndexValid(p_currentRoomIndex)) {
			return nullptr;
		}
		return p_rooms[p_currentRoomIndex];
	}

	Room* RoomVector::gotoNextRoom() {
		auto currentRoom = getCurrentRoom();
		if (currentRoom && isIndexValid(currentRoom->getNextIndex())) {
			static std::vector<void*> ptrs = currentRoom->E_exit();
			currentRoom->setNotActive();
			p_currentRoomIndex = currentRoom->getNextIndex();
			currentRoom = getCurrentRoom();
			currentRoom->E_entry(ptrs);
			currentRoom->setActive();
		}
		return currentRoom;
	}

	Room* RoomVector::gotoPrevRoom() {
		auto currentRoom = getCurrentRoom();
		if (currentRoom && isIndexValid(currentRoom->getPrevIndex())) {
			static std::vector<void*> ptrs = currentRoom->E_exit();
			currentRoom->setNotActive();
			p_currentRoomIndex = currentRoom->getPrevIndex();
			currentRoom = getCurrentRoom();
			currentRoom->E_entry(ptrs);
			currentRoom->setActive();
		}
		return currentRoom;
	}

	Room* RoomVector::gotoRoom(int index) {
		auto currentRoom = getCurrentRoom();
		if (isIndexValid(index)) {
			static std::vector<void*> ptrs;
			if (currentRoom) {
				ptrs = currentRoom->E_exit();
				currentRoom->setNotActive();
			}
			p_currentRoomIndex = index;
			currentRoom = getCurrentRoom();
			currentRoom->E_entry(ptrs);
			currentRoom->setActive();
			currentRoom->E_step();
		}
		return currentRoom;
	}

	Room* RoomVector::gotoRoom(int index, std::vector<void*> enterPointers) {
		auto currentRoom = getCurrentRoom();
		if (isIndexValid(index)) {
			if (currentRoom) {
				currentRoom->E_exit();
				currentRoom->setNotActive();
			}
			p_currentRoomIndex = index;
			currentRoom = getCurrentRoom();
			currentRoom->E_entry(enterPointers);
			currentRoom->setActive();
		}
		return currentRoom;
	}

	void RoomVector::addRoom(Room* room, const std::vector<std::string>& buildArgs) {
		room->p_roomIndex = (int)p_rooms.size();
		p_rooms.push_back(room);
		room->setParentMainProgram(p_program);
		room->E_build(buildArgs);
	}

	void RoomVector::destroy() {
		for (size_t i = 0; i < p_rooms.size(); i++) {
			p_rooms[i]->E_destroy();
		}
		p_rooms.resize(0);
		p_currentRoomIndex = Room::NO_ROOM_INDEX;
	}

	bool RoomVector::isIndexValid(int index) const {
		if (index == Room::NO_ROOM_INDEX || index < 0 || index >= (int)p_rooms.size()) {
			return false;
		}
		return true;
	}

}