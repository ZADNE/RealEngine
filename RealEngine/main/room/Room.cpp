#include <RealEngine/main/room/Room.hpp>

namespace RE {

Room::Room() {

}

Room::~Room() {

}

const RoomDisplaySettings& Room::getDisplaySettings() {
	static RoomDisplaySettings settings{};
	return settings;
}

void Room::windowResized(const glm::ivec2& newSize) {

}

MainProgram* Room::program() const {
	return m_mainProgram;
}

const InputManager* Room::input() const {
	return m_inputManager;
}

Synchronizer* Room::synchronizer() const {
	return m_synchronizer;
}

Window* Room::window() const {
	return m_window;
}

}