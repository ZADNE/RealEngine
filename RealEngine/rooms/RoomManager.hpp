/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <memory>
#include <utility>

#include <glm/vec2.hpp>

#include <RealEngine/rooms/RoomTransitionArguments.hpp>

namespace re {

class Room;

template<class T>
concept DerivedFromRoom = std::derived_from<T, Room>;

/**
 * @brief Manages rooms and transitions among them.
 */
class RoomManager {
public:
    /**
     * @brief Contructs room manager without any rooms to manage.
     */
    RoomManager();

    /**
     * @brief Gets the current room, that is the room which has active session.
     * @return Pointer to current room, nullptr before first room is entered.
     */
    Room* currentRoom() const;

    /**
     * @brief Changes the current room.
     * @details Session of current room is ended and then session
     * of the room with name 'name' is started.
     * @param name Identifier of the room
     * @param args Arguments to start the room's session with
     * @return  Pointer to active room, this can be same as previous room
     *          if the name is invalid.
     */
    Room* goToRoom(size_t name, const RoomTransitionArguments& args);

    /**
     * @brief Adds new room to the manager.
     *
     * Room of the given type (which has to be derived from Room)
     * is constructed using the given arguments.
     *
     * RoomManager holds ownership of the rooms.
     */
    template<DerivedFromRoom RoomType, typename... ConstructorArgs>
    RoomType* addRoom(ConstructorArgs&&... args) {
        auto ptr = std::make_unique<RoomType>(std::forward<ConstructorArgs>(args
        )...);
        auto* rval = ptr.get();
        m_rooms.push_back(std::move(ptr));
        return rval;
    }

    /**
     * @brief Calls given Room-member function on all rooms
     * @tparam callback Member function of Room
     */
    template<auto callback, typename... Args>
    void notifyRooms(Args... args) const {
        for (auto& room : m_rooms) { ((*room).*callback)(args...); }
    }

protected:
    std::vector<std::unique_ptr<Room>> m_rooms; /**< Contains all managed rooms */
    Room* m_currentRoom = nullptr; /**< Pointer to the current room */
};

} // namespace re