/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>

#include <RealEngine/utility/Version.hpp>

/** @mainpage RealEngine
 *
 * @section overview Overview
 *
 * To get a minimal working RealEngine application, you need to create your own
 * room class that inherits from Room. The typical main function then looks like
 * this: @code {cpp} int main(int argc, char* argv[]) {
 *      MainProgram::initialize();
 *      auto* myRoom = MainProgram::addRoom<MyDerivedRoom>(constructorArgs);
 *      return MainProgram::run(myRoom->name(), transitionparameters);
 *  }
 * @endcode
 */

/**
 * @namespace re
 * @brief Everything provided by RealEngine is inside this namespace
 */

/**
 * @namespace re::glsl
 * @brief Contains exposed GLSL shaders, functions and types
 */

/**
 * @namespace re::details
 * @brief Not to be used directly, used for internal implementation
 */

namespace re {

/**
 * @brief Represents RealEngine's subsystems
 * @note This is used internally within Window.
 */
class WindowSubsystems {
public:
    /**
     * @brief Initializes RealEngine's subsystems
     * @throws std::runtime_error When a system failed to initialize.
     */
    WindowSubsystems();

    WindowSubsystems(const WindowSubsystems&) = delete; ///< Noncopyable
    WindowSubsystems& operator=(const WindowSubsystems&) = delete; ///< Noncopyable

    WindowSubsystems(WindowSubsystems&&)            = delete; ///< Nonmovable
    WindowSubsystems& operator=(WindowSubsystems&&) = delete; ///< Nonmovable

    ~WindowSubsystems();

    /**
     * @brief Gets human readable string with the version of RealEngine
     * @return Human readable version string
     */
    static const std::string& RealEngineVersionString() {
        using namespace std::string_literals;
        const static std::string str =
            "RealEngine:   "s + std::to_string(k_versionMajor) + "."s +
            std::to_string(k_versionMinor) + "."s + std::to_string(k_versionPatch);
        return str;
    }

    static void printRealEngineVersion();

    void printSubsystemsVersions() const;
};

} // namespace re
