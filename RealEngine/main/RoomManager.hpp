#pragma once
#include <vector>
#include <string>
#include <memory>

#include <RealEngine/main/CommandLineArguments.hpp>
#include <RealEngine/main/Room.hpp>

namespace RE {

class RoomManager {
public:
	RoomManager();
	~RoomManager();

	//Returns nullptr if there is no "current room" (should not happen)
	Room* getCurrentRoom() const;

	//Returns new current room - that can be the same room as before (could not move because of bad index)
	//enterPointers are used instead of the returned ones from the previous room
	Room* gotoRoom(size_t index);

	template<class T>
	Room* addRoom(CommandLineArguments args) {
		p_rooms.emplace_back(std::make_unique<T>(args));
		return p_rooms.back().get();
	}
protected:
	bool isIndexValid(size_t index) const;

	std::vector<std::unique_ptr<Room>> p_rooms;
	Room* p_currentRoom = nullptr;
};

}