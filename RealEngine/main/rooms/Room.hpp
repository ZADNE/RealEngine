/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <RealEngine/main/rooms/RoomSystemAccess.hpp>
#include <RealEngine/main/rooms/RoomDisplaySettings.hpp>
#include <RealEngine/main/rooms/RoomTransitionParameters.hpp>

namespace RE {

class InputManager;

/**
 * @brief Separates program into logical units, only one room is active at a time.
 *
 * Rooms allow separation of program into multiple units where only one unit
 * is active at a time (= receives step and render calls, inactive rooms
 * are resident in the memory but do not receive updates).
 * Transitions between rooms are mamaged by RoomManager.
 *
 * @see RoomManager
*/
class Room {
public:

	/**
	 * @brief Constructs new room
	 * @param name Unique identifier of the room
	*/
	Room(size_t name, RoomDisplaySettings initialSettings = RoomDisplaySettings{});

	/**
	 * @brief Destructs a room.
	 *
	 * This happens when the program ends.
	*/
	virtual ~Room() = default;

	Room(const Room&) = delete;
	Room& operator=(const Room&) = delete;

	/**
	 * @brief Informs the room that a new session happens inside it.
	 * @param params Parameters to initialize the session. These are
	 *				 are obtained from previous room when it requests
	 *				 transition to this room.
	*/
	virtual void sessionStart(const RoomTransitionParameters& params) = 0;

	/**
	 * @brief Informs the room that its session ends.
	 *
	 * This happens after it was requested from the main program.
	*/
	virtual void sessionEnd() = 0;

	/**
	 * @brief The main update function of the room.
	 *
	 * This function is called at fixed rate per second.
	 * The rate can be changed via Synchronizer.
	*/
	virtual void step() = 0;

	/**
	 * @brief This is the function where rendering should happen.
	 *
	 * This function is called at a variable rate, depending on
	 * the speed of hardware. Upper limit can be set via Synchronizer.
	 *
	 * @param interpolationFactor Represents relative time between last
	 *							  performed step and the upcoming step.
	 *							  Use it to interpolate between discrete
	 *							  simulation steps.
	*/
	virtual void render(double interpolationFactor) = 0;

	/**
	 * @brief Callback used to notify that the window's size has changed
	*/
	virtual void windowResizedCallback(const glm::ivec2& oldSize, const glm::ivec2& newSize) {}

	/**
	 * @brief Gets a proxy that can be used to read/modify
	 * many parameters of the program at run-time
	*/
	static RoomSystemAccess& system() { return *s_systemAccess; }

	/**
	 * @brief Allows access to mouse & keyboard user input
	*/
	static const InputManager& input() { return *s_inputManager; }

	/**
	 * @brief Gets the settings that should be used for this room
	*/
	RoomDisplaySettings getDisplaySettings() const;

	/**
	 * @brief Gets unique identifier of the room
	*/
	size_t getName() const;

	/**
	 * @brief This is set by the MainProgram at startup
	*/
	static void setRoomSystemAccess(RoomSystemAccess* systemAccess) { s_systemAccess = systemAccess; }

	/**
	 * @brief This is set by the MainProgram at startup
	*/
	static void setInputManager(InputManager* inputManager) { s_inputManager = inputManager; }

	/**
	 * @brief This is set by the MainProgram at startup
	*/
	static void setRoomManager(RoomManager* roomManager) { s_roomManager = roomManager; }

private:

	/**
	 * @brief Dynamically changes the room's display settings
	*/
	void setDisplaySettings(RoomDisplaySettings displaySettings);

	RoomDisplaySettings m_displaySettings;
	size_t m_name;

	inline static RoomSystemAccess* s_systemAccess = nullptr;
	inline static const InputManager* s_inputManager = nullptr;
	inline static RoomManager* s_roomManager = nullptr;
};

}