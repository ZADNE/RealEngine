#pragma once
#include <vector>
#include <string>

namespace RE {

	class Room;
	class MainProgram;

	class RoomVector{

	public:
		RoomVector(MainProgram* mainProgram);
		~RoomVector();

			//Returns nullptr if there is no "current room" (should not happen)
		Room* getCurrentRoom() const;

			//Returns new current room - that can be the same room as before (could not move because of bad index)
		Room* gotoNextRoom();
			//Returns new current room - that can be the same room as before (could not move because of bad index)
		Room* gotoPrevRoom();
			//Returns new current room - that can be the same room as before (could not move because of bad index)
		Room* gotoRoom(int index);
			//Returns new current room - that can be the same room as before (could not move because of bad index)
			//enterPointers are used instead of the returned ones from the previous room
		Room* gotoRoom(int index, std::vector<void*> enterPointers);

		void addRoom(Room* room, const std::vector<std::string>& buildArgs);

		void destroy();
	protected:
		bool isIndexValid(int index) const;

		std::vector<Room*> p_rooms;
		int p_currentRoomIndex = -1;
		MainProgram* p_program = nullptr;
	};

}