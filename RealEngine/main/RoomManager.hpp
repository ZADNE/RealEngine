#pragma once
#include <vector>
#include <string>
#include <memory>

#include <RealEngine/main/CommandLineArguments.hpp>
#include <RealEngine/main/Room.hpp>

namespace RE {

/**
 * @brief Manages rooms and transitions among them.
*/
class RoomManager {
public:
	/**
	 * @brief Contructs room manager without rooms.
	*/
	RoomManager();

	/**
	 * @brief Destructs room manager and all the rooms it manages.
	*/
	~RoomManager();

	/**
	 * @brief Gets current room, that is the room which has active session.
	 * @return Pointer to current room, nullptr before first room is entered.
	*/
	Room* getCurrentRoom() const;

	/**
	 * @brief Changes current room. That means ends session of current room
	 * and start session of the room at index.
	 *
	 * @param index Index of the room to enter
	 * @param params Parameters to start the room's session with
	 * @return	Pointer to active room, this can be same as previous room
	 *			if the index is invalid.
	*/
	Room* gotoRoom(size_t index, const RoomTransitionParameters& params);

	/**
	 * @brief Adds new room to the manager.
	 *
	 * This is typically called from the constructor of your
	 * class derived from MainProgram.
	 *
	 * @tparam T Room derived from RE::Room
	 * @param args Command line arguments to construct the room with.
	 * @return Pointer to the new room
	*/
	template<class T>
	Room* addRoom(CommandLineArguments args) {
		p_rooms.emplace_back(std::make_unique<T>(args));
		return p_rooms.back().get();
	}

protected:
	std::vector<std::unique_ptr<Room>> p_rooms;	/**< Vector of all managed rooms */
	Room* p_currentRoom = nullptr;				/**< Pointer to current room */
};

}