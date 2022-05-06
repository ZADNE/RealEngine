/*! 
 *  @author    Dubsky Tomas
 */
#pragma once

#include <string>

static const int RE_VERSION_MAJOR = 1;
static const int RE_VERSION_MINOR = 8;
static const int RE_VERSION_PATCH = 0;

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
 * @brief Represents RealEngine's subsystems
*/
class RealEngine {
public:
	/**
	 * @brief Initializes all RealEngine's subsystems
	 *
	 * The internal subsystem initialized are SDL2,
	 * and some OpenGL context flags.
	 *
	 * @throws int when a system failed to initialize
	*/
	RealEngine();

	/**
	 * @brief De-initializes all RealEngine's subsystems
	*/
	~RealEngine();

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
};


}