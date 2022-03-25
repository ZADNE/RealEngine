/**
 * @file
 * @author Tomáš Dubský
*/
#pragma once
#include <SDL2/SDL_main.h>

#include <glm/vec2.hpp>

#include <RealEngine/main/RealEngine.hpp>
#include <RealEngine/main/RoomManager.hpp>
#include <RealEngine/main/Window.hpp>
#include <RealEngine/user_input/InputManager.hpp>
#include <RealEngine/main/Synchronizer.hpp>

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
 * @brief Use this function to run a RealEngine program.
 *
 * Typical RealEngine main function should look like this:
 * @code
 * int main(int argc, char* argv[]) {
 *     return RE::runProgram<MyDerivedFromREMainProgram>(argc, argv);
 * }
 * @endcode
 *
 * @tparam T Class derived from MainProgram
 * @param argc Number of arguments passed to the program from the environment
 * @param argv Pointer to the first element of an array of C-strings that represent program's arguments
 * @return The exit code that should be returned from main.
*/
template<class T>
int runProgram(int argc, char* argv[]) {
	//Contruct command line arguments
	CommandLineArguments args = std::span(argv, argc);

	try {
		//Construct the program
		static T program{args};

		//And run it
		return program.run();
	}
	catch (const std::exception& e) {
		fatalError(std::string("Exception: ") + e.what());
	}
	catch (const char* str) {
		fatalError(std::string("C-string exception: ") + str);
	}
	catch (int i) {
		fatalError(std::string("int exception: ") + std::to_string(i));
	}
	catch (...) {
		fatalError("Unknown exception!");
	}
}


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
class MainProgram {
private:
	RealEngine m_realEngine;/**< Empty class that initializes and de-initializes systems */
public:
	MainProgram(const MainProgram& other) = delete;
	void operator=(const MainProgram& other) = delete;

	/**
	 * @brief Runs the program
	 *
	 * The return value represent the exit code that this
	 * RealEngine program should return to the enviroment.
	 * The exit code can be altered by scheduleProgramExit().
	 *
	 * @return The exit code that should be returned to the enviroment.
	*/
	int run();

	/**
	 * @brief Schedules the program to exit.
	 *
	 * The program will end at the end of the current frame.
	 *
	 * @param exitcode The exit code that should be returned from run()
	*/
	void scheduleProgramExit(int exitcode = EXIT_SUCCESS);

	/**
	 * @brief Schedules transition to another ('next') room.
	 *
	 * The transition, which happens at the end of current frame,
	 * does following:
	 * - ends session of current room via sessionEnd()
	 * - start session of next room via sessionStart(params)
	 * - ensures that at least one step() happens in the next room before render()
	 *
	 * @param index The index of next room, no transition will happen if the index is invalid.
	 * @param params Parameters to start the next room's session with.
	*/
	void scheduleRoomTransition(size_t index, RoomTransitionParameters params);

	/**
	 * @brief Pointer to main program
	 * @deprecated Use program() within rooms instead.
	*/
	static MainProgram* std;

	void checkForInput(bool check);

	/**
	 * @brief Gets displays that can be drawn to
	*/
	std::vector<RE::DisplayInfo> getDisplays() const;

	/**
	 * @brief Resizes the window and notifies all room of the change.
	 * @param newDims New dimensions of the window
	 * @param save Changed settings are saved to file if true.
	*/
	void resizeWindow(const glm::ivec2& newDims, bool save);
protected:
	/**
	 * @brief Constructs main program.
	 *
	 * Do not forget to add rooms to room manager
	 * and to enter a room inside your derived class
	 * constructor or the program ends immediately!
	*/
	MainProgram();

	/**
	 * @brief Destructs the main program
	*/
	virtual ~MainProgram();

	RoomManager p_roomManager; /**< Manages rooms - you have to add at least 1 room and enter it in the contructor of your derived class */
	Window p_window{WindowSettings{}, RealEngine::getVersion()}; /**< window also creates and initializes OpenGL context */
	InputManager p_inputManager; /**< Record key presses/releases, mouse movement etc. */
	Synchronizer p_synchronizer{50u, 50u}; /**< Maintains constant speed of simulation, can also limit FPS */
private:
	void step();
	void render(double interpolationFactor);

	void pollEvents();
	void processEvent(SDL_Event* evnt);

	void adoptRoomSettings(const Room::DisplaySettings& s);

	bool m_programShouldRun = false;
	int m_programExitCode = EXIT_SUCCESS;

	bool m_checkForInput = true;
	bool m_usingImGui = false;

	void doRoomTransitionIfScheduled();

	static const size_t NO_NEXT_ROOM = std::numeric_limits<size_t>::max();
	size_t m_nextRoomIndex = NO_NEXT_ROOM;
	RoomTransitionParameters m_roomTransitionParameters;
};

}
