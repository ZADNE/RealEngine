#pragma once
#include <string>
#include <vector>

#include <RealEngine/main/MainProgram.hpp>

namespace RE {

enum class RoomState { NOT_ACTIVE, ACTIVE, EXIT_PROGRAM, CHANGE_TO_NEXT, CHANGE_TO_PREV };


class Room {
	friend class RoomVector;
public:
	static const int NO_ROOM_INDEX = -1;

	Room();
	virtual ~Room();

	virtual void E_build(const std::vector<std::string>& buildArgs) = 0;
	virtual void E_destroy() = 0;

	//Entry arguments are taken from previous room's leave arguments
	virtual void E_entry(std::vector<void*> enterPointers) = 0;

	//It is supposed to return p_leavePointers
	virtual std::vector<void*> E_exit() = 0;

	virtual void E_step() = 0;
	virtual void E_draw(double interpolationFactor) = 0;

	int getMyIndex();

	void setActive() {
		p_state = RoomState::ACTIVE;
	};

	void setNotActive() {
		p_state = RoomState::NOT_ACTIVE;
	};

	RoomState getState() const {
		return p_state;
	};

	virtual int getNextIndex() const {
		return NO_ROOM_INDEX;
	};

	virtual int getPrevIndex() const {
		return NO_ROOM_INDEX;
	};

	void setParentMainProgram(MainProgram* mainProgram) { p_MP = mainProgram; };
protected:
	RoomState p_state = RoomState::NOT_ACTIVE;
	int p_roomIndex = NO_ROOM_INDEX;
	MainProgram* p_MP = nullptr;

	std::vector<void*> p_leavePointers;
};

}