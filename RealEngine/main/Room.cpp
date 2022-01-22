#include <RealEngine/main/Room.hpp>

namespace RE {

Room::Room() {

}

Room::~Room() {

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