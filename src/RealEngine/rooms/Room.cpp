/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/program/MainProgram.hpp>
#include <RealEngine/rooms/Room.hpp>

namespace re {

Room::Room(size_t name, RoomDisplaySettings initialSettings /* = RoomDisplaySettings{}*/)
    : m_displaySettings(initialSettings)
    , m_name(name) {
}

void Room::setDisplaySettings(const RoomDisplaySettings& displaySettings) {
    m_displaySettings = displaySettings;
    if (s_roomManager->currentRoom() == this) { // If this room is active
        // Notify MainProgram
        s_mainProgram->adoptRoomDisplaySettings(displaySettings);
    }
}

void Room::setStaticReferences(MainProgram* mainProgram, RoomManager* roomManager) {
    s_mainProgram = mainProgram;
    s_roomManager = roomManager;
}

} // namespace re