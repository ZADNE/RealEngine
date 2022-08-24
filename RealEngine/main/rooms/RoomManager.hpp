/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <unordered_map>

#include <glm/vec2.hpp>

#include <RealEngine/main/rooms/RoomTransitionParameters.hpp>

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
	 * @brief Changes the current room.
	 * @details Session of current room is ended and then session
	 * of the room with name 'name' is started.
	 * @param name Identifier of the room
	 * @param params Parameters to start the room's session with
	 * @return	Pointer to active room, this can be same as previous room
	 *			if the name is invalid.
	*/
	Room* goToRoom(size_t name, const RoomTransitionParameters& params);

	/**
	 * @brief Adds new room to the manager.
	 *
	 * The room manager does not hold ownership of the room
	 * and it is your responsibility that the pointer is valid through
	 * the lifetime of the room manger.
	 *
	 * This is automatically called when a Room is constructed
	*/
	void addRoom(Room* room);

	/**
	 * @brief Calls given Room-member function on all rooms
	 * @tparam callback Member function of Room
	*/
	template<auto callback, typename... Args>
	void notifyRooms(Args... args) {
		for (auto& room : m_rooms) {
			(room.second->*callback)(args...);
		}
	}

protected:

	std::unordered_map<size_t, Room*> m_rooms;				/**< Non-owning vector of all managed rooms */
	Room* m_currentRoom = nullptr;							/**< Pointer to the current room */
};

}