/*!
 *  @author    Dubsky Tomas
 */
#pragma once

#include <string>

constexpr int RE_VERSION_MAJOR = 1;
constexpr int RE_VERSION_MINOR = 9;
constexpr int RE_VERSION_PATCH = 0;

/*! \mainpage RealEngine
 *
 * \section overview Overview
 *
 * To get a minimal working RealEngine application, two of its classes have to be inherited.
 *	*	At least one room which inherits from RE::Room has to be created.
 *  *	A custom program class which inherits from RE::MainProgram also has to be created.
 *		MainProgram initializes all subsystems and maintains the simulation. The room should be entered in its constructor.
 *
 * The whole application then should be started by RE::runProgram() function.
 */


namespace RE {

/**
 * @brief Lists renderers known to RealEngine
*/
enum class Renderer {
	OPENGL_46		/**< Open Graphics Library 4.6 renderer */
};

std::string to_string(Renderer r);

/**
 * @brief Represents RealEngine's subsystems
*/
class WindowSubsystems {
	friend class Window;
public:

	WindowSubsystems(const WindowSubsystems&) = delete;
	WindowSubsystems& operator=(const WindowSubsystems&) = delete;

	/**
	 * @brief Gets human readable string with the version of RealEngine
	 * @return Human readable version string
	*/
	static const std::string& getVersion() {
		using namespace std::string_literals;
		const static std::string str = "RealEngine:   "s
			+ std::to_string(RE_VERSION_MAJOR) + "."s
			+ std::to_string(RE_VERSION_MINOR) + "."s
			+ std::to_string(RE_VERSION_PATCH);
		return str;
	}

private:

	/**
	 * @brief Initializes all RealEngine's subsystems
	 *
	 * The internal subsystem initialized are SDL2 and renderer.
	 *
	 * @throws int when a system failed to initialize
	*/
	WindowSubsystems(Renderer renderer);

	/**
	 * @brief De-initializes all RealEngine's subsystems
	*/
	~WindowSubsystems();
};


}