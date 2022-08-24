/*!
 *  @author    Dubsky Tomas
 *	@file
 */
#pragma once
#include <concepts>

#include <SDL2/SDL_main.h>

#include <glm/vec2.hpp>

#include <RealEngine/utility/error.hpp>
#include <RealEngine/main/rooms/RoomSystemAccess.hpp>
#include <RealEngine/main/rooms/RoomManager.hpp>
#include <RealEngine/main/rooms/RoomDisplaySettings.hpp>
#include <RealEngine/main/rooms/RoomTransitionParameters.hpp>
#include <RealEngine/main/window/Window.hpp>
#include <RealEngine/main/program/Synchronizer.hpp>
#include <RealEngine/user_input/InputManager.hpp>

union SDL_Event;

namespace RE {

class RoomVector;
class Room;

struct DisplayInfo {
	std::string name; /**< @brief UTF-8 encoded 'name' */
	glm::ivec4 bounds; /**< @brief Bounds in pixels; XYWH */
	glm::ivec4 boundsUsable; /**< @brief Usable bounds in pixels; XYWH; system-reserved parts removed */
	glm::ivec2 dims; /**< @brief Dimensions */
	int refreshRate; /**< @brief Refresh rate */
	Uint32 pixelFormat; /**< @brief Pixel format @see SDL2's SDL_PixelFormatEnum */
	void* driverSpecific;
};

/**
 * @brief This is the centre point of the whole application.
 *
 * This class should not be instantiated directly (use runProgram() function
 * to start a RealEngine program) but this class should be used as superclass
 * for your own program class that will be instantiated by the above function.
 *
 * This class encapsulates many functionalities that are internally delegated to a number of subclasses.
 *
 * @see RealEngine
 * @see RoomManager
 * @see Window
 * @see InputManager
 * @see Synchronizer
*/
class MainProgram final {
public:

	MainProgram(const MainProgram&) = delete;
	void operator=(const MainProgram&) = delete;

	/**
	 * @brief Must be called before any Room is constructed
	*/
	static void initialize();

	/**
	 * @brief Runs the program
	 *
	 * The return value represent the exit code that this
	 * RealEngine program should return to the enviroment.
	 * The exit code can be altered by scheduleExit().
	 *
	 * @param room The first room to enter
	 * @param params Parameters that the room will be entered with
	 * @return The exit code that should be returned to the enviroment.
	*/
	static int run(Room& room, const RoomTransitionParameters& params);

	/**
	 * @brief Schedules the program to exit.
	 *
	 * The program will end at the end of the current frame.
	 *
	 * @param exitcode The exit code that should be returned from run()
	*/
	void scheduleExit(int exitcode = EXIT_SUCCESS);

	/**
	 * @brief Schedules transition to another ('next') room.
	 *
	 * The transition, which happens at the end of current frame,
	 * does following:
	 * - ends session of current room via sessionEnd()
	 * - start session of next room via sessionStart(params)
	 * - ensures that at least one step() happens in the next room before render()
	 *
	 * @param name Name of the next room, no transition will happen if there is no room with such name.
	 * @param params Parameters to start the next room's session with.
	*/
	void scheduleRoomTransition(size_t name, RoomTransitionParameters params);

	/**
	 * @brief Pointer to main program
	 *
	 * Use program() within rooms instead.
	*/
	static MainProgram* std;

	void checkForInput(bool check);

	/**
	 * @brief Gets displays that can be drawn to
	*/
	std::vector<RE::DisplayInfo> getDisplays() const;

	void setRelativeCursorMode(bool relative);

private:

	/**
	 * @brief Constructs the main program.
	*/
	MainProgram();

	/**
	 * @brief Gets the singleton instance
	*/
	static MainProgram& instance();

	/**
	 * @brief Does the actual game loop on the singleton instance
	*/
	int doRun(Room& room, const RoomTransitionParameters& params);

	void step();
	void render(double interpolationFactor);

	void pollEvents();
	void processEvent(SDL_Event* evnt);

	void adoptRoomSettings(const RoomDisplaySettings& rds);

	Window m_window{WindowSettings{}, WindowSubsystems::getVersion()}; /**< Window also creates and initializes OpenGL context */
	RoomManager m_roomManager; /**< Manages rooms - you have to add at least 1 room and enter it in the contructor of your derived class */
	InputManager m_inputManager; /**< Record key presses/releases, mouse movement etc. */
	Synchronizer m_synchronizer{50u, 50u}; /**< Maintains constant speed of simulation, can also limit FPS */
	RoomSystemAccess m_roomSystemAccess;

	bool m_programShouldRun = false;
	int m_programExitCode = EXIT_SUCCESS;

	bool m_checkForInput = true;
	bool m_usingImGui = false;
	glm::vec4 m_clearColor{};

	void doRoomTransitionIfScheduled();

	static constexpr size_t NO_NEXT_ROOM = std::numeric_limits<size_t>::max();
	size_t m_nextRoomName = NO_NEXT_ROOM;
	RoomTransitionParameters m_roomTransitionParameters;
};

}
