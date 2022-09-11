/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rooms/RoomManager.hpp>

#include <RealEngine/rooms/Room.hpp>


namespace RE {

RoomManager::RoomManager() {

}

Room* RoomManager::getCurrentRoom() const {
    return m_currentRoom;
}

Room* RoomManager::goToRoom(size_t name, const RoomTransitionArguments& args) {
    for (auto& room : m_rooms) {//If name is valid
        if (room->getName() == name) {
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