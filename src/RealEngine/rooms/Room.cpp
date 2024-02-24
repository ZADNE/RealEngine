/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/program/MainProgram.hpp>
#include <RealEngine/rooms/Room.hpp>

namespace re {

Room::Room(size_t name, RoomDisplaySettings initialSettings /* = RoomDisplaySettings{}*/)
    : m_displaySettings{initialSettings}
    , m_name{name}
    , m_mainRenderPass{*initialSettings.mainRenderPass} {
}

void Room::setStaticReferences(MainProgram* mainProgram, RoomManager* roomManager) {
    s_mainProgram = mainProgram;
    s_roomManager = roomManager;
}

void Room::setDisplaySettings(const RoomDisplaySettings& displaySettings) {
    assert(
        (m_displaySettings.mainRenderPass == displaySettings.mainRenderPass) &&
        "Dynamic changing of main RenderPass is not supported"
    );
    m_displaySettings = displaySettings;
    if (s_roomManager->currentRoom() == this) { // If this room is active
        // Notify MainProgram
        s_mainProgram->adoptRoomDisplaySettings(displaySettings);
    }
}

} // namespace re