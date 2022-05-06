/*! 
 *  \author    Dubsky Tomas
 */
#pragma once

#include <string>

static const int RE_VERSION_MAJOR = 1;
static const int RE_VERSION_MINOR = 8;
static const int RE_VERSION_PATCH = 0;


namespace RE {

/**
 * @brief Class that logically represents RealEngine's subsystems
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