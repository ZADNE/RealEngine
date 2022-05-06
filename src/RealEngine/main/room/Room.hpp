/*! 
 *  \author    Dubsky Tomas
 */
#pragma once
#include <string>
#include <vector>
#include <any>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <RealEngine/main/MainProgram.hpp>

namespace RE {

class MainProgram;

/**
 * @brief Separates program into logical units, only one unit is active at a time.
 *
 * Rooms allow separation of program into multiple units where only one unit
 * is active at a time (= receives step and render calls, inactive rooms
 * are resident in the memory but do not receive updates).
 * Transitions between rooms are mamaged by RoomManager.
 *
 * @see RoomManager
*/
class Room {
	friend class MainProgram;
public:

	/**
	 * @brief Constructs a room.
	 *
	 * This happens when the room is added to the manager.
	*/
	Room();

	/**
	 * @brief Destructs a room.
	 *
	 * This happens when the program ends.
	*/
	virtual ~Room();

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
	 * @brief Gets the settings that will be used for this room
	 * 
	 * The settings are adopted every time the room is entered
	*/
	virtual const RoomDisplaySettings& getDisplaySettings();

	/**
	 * @brief Callback used to notify that the window's size has changed
	 * 
	 * This callback is called for all rooms, even those that are not active.
	 * @param newSize The new size fo the window
	*/
	virtual void windowResized(const glm::ivec2& newSize);

	/**
	 * @brief Gets main program.
	 * @return Main program, guaranteed to be valid.
	*/
	MainProgram* program() const;

	/**
	 * @brief Gets input manager which can be used to determine
	 * user input (keyboard, mouse, etc.).
	 *
	 * @return Input manager, guaranteed to be valid.
	*/
	const InputManager* input() const;

	/**
	 * @brief Gets synchronizer which can be used to change rate
	 * of steps or to limit render rates.
	 *
	 * @return Synchronizer, guaranteed to be valid.
	*/
	Synchronizer* synchronizer() const;

	/**
	 * @brief Gets window
	 * @return Window , guaranteed to be valid.
	*/
	Window* window() const;
private:
	inline static MainProgram* m_mainProgram = nullptr;			/**< Pointer set by main program */
	inline static const InputManager* m_inputManager = nullptr;	/**< Pointer set by main program */
	inline static Synchronizer* m_synchronizer = nullptr;		/**< Pointer set by main program */
	inline static Window* m_window = nullptr;					/**< Pointer set by main program */
};

}