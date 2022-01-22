#pragma once

#include <string>

static const int RE_VERSION_MAJOR = 1;
static const int RE_VERSION_MINOR = 7;
static const int RE_VERSION_PATCH = 1;


namespace RE {

/**
 * @brief Class that logically represents RealEngine's subsystems
*/
class RealEngine {
public:
	/**
	 * @brief Initializes all RealEngine's subsystems
	 *
	 * The internal subsystem initialized are SDL2, SDL2_TTF,
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