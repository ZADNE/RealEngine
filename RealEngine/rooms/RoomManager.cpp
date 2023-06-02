/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rooms/RoomManager.hpp>

#include <RealEngine/rooms/Room.hpp>


namespace re {

RoomManager::RoomManager() {

}

Room* RoomManager::currentRoom() const {
    return m_currentRoom;
}

Room* RoomManager::goToRoom(size_t name, const RoomTransitionArguments& args) {
    for (auto& room : m_rooms) {//If name is valid
        if (room->name() == name) {
            if (m_currentRoom) {
                m_currentRoom->sessionEnd();//End session of the current room
            }
            m_currentRoom = room.get();
            m_currentRoom->sessionStart(args);//And start its session
        }
    }
    return m_currentRoom;
}

}