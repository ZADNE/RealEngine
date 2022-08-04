/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vector>

#include <glm/vec2.hpp>

#include <RealEngine/main/CommandLineArguments.hpp>
#include <RealEngine/main/room/RoomTransitionParameters.hpp>

namespace RE {

class Room;

/**
 * @brief Manages rooms and transitions among them.
 *
 * Room manager does not hold ownership of the rooms it manages.
*/
class RoomManager {
public:
	/**
	 * @brief Contructs room manager without any rooms to manage.
	*/
	RoomManager();

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
	 * The room manager does not hold ownership of the room
	 * and it is your responsibility that the point is valid through
	 * lifetime of the room manger.
	 *
	 * This is typically called from the constructor of the
	 * class derived from the MainProgram.
	 * @param room Room to manage
	 * @return Index of the room, can be used to enter the room via gotoRoom()
	*/
	size_t addRoom(Room* room);

	/**
	 * @brief Notifies all rooms that the window has been resized
	 * @param newSize The new size of the window
	*/
	void notifyWindowResized(const glm::ivec2& newSize);

protected:
	std::vector<Room*> m_rooms;								/**< Non-owning vector of all managed rooms */
	Room* m_currentRoom = nullptr;							/**< Pointer to current room */
};

}