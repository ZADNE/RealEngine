#pragma once
#include <string>
#include <vector>

#include <RealEngine/main/CommandLineArguments.hpp>

namespace RE {

class MainProgram;
class InputManager;
class Synchronizer;
class Window;

using RoomTransitionParameters = std::vector<void*>;

class Room {
	friend class MainProgram;
public:

	Room();
	virtual ~Room();

	//Entry arguments are taken from previous room's leave arguments
	virtual void E_entry(RoomTransitionParameters params) = 0;

	//It is supposed to return p_leavePointers
	virtual RoomTransitionParameters E_exit() = 0;

	virtual void E_step() = 0;
	virtual void E_draw(double interpolationFactor) = 0;

	MainProgram* program() const;

	const InputManager* input() const;

	Synchronizer* synchronizer() const;

	Window* window() const;
private:
	inline static MainProgram* m_mainProgram = nullptr;
	inline static const InputManager* m_inputManager = nullptr;
	inline static Synchronizer* m_synchronizer = nullptr;
	inline static Window* m_window = nullptr;
};

}