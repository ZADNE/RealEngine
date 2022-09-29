/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>

 /*! \mainpage RealEngine
  *
  * \section overview Overview
  *
  * To get a minimal working RealEngine application, you need to create your own room class
  * that inherits from RE::Room. The typical main function then looks like this:
  * \code {cpp}
  *  int main(int argc, char* argv[]) {
  *      RE::MainProgram::initialize();
  *      auto* myRoom = RE::MainProgram::addRoom<MyDerivedRoom>(constructorArgs);
  *      return RE::MainProgram::run(myRoom->getName(), transitionparameters);
  *  }
  * \endcode
 */


namespace RE {

constexpr int RE_VERSION_MAJOR = 1;
constexpr int RE_VERSION_MINOR = 9;
constexpr int RE_VERSION_PATCH = 1;

constexpr int RE_VERSION = RE_VERSION_MAJOR * 1e6 + RE_VERSION_MINOR * 1e3 + RE_VERSION_PATCH;

/**
 * @brief Lists all renderers known to RealEngine
 *
 * The order also represents precedence when no specific renderer is preffered
 * (or if it fails during initialization).
*/
enum class RendererID {
    VULKAN13,       /**< Vulkan 1.3 renderer */
    OPENGL46,       /**< Open Graphics Library 4.6 renderer */
    ANY
};

std::string to_string(RendererID r);

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

    static void printRealEngineVersion();

    void printSubsystemsVersions() const;

private:

    /**
     * @brief Is a very simple RAII wrapper around SDL2
    */
    class SDL2_RAII {
    public:

        SDL2_RAII();

        SDL2_RAII(const SDL2_RAII&) = delete;
        SDL2_RAII& operator=(const SDL2_RAII&) = delete;

        ~SDL2_RAII();

        void printVersion() const;
    };

    SDL2_RAII m_sdl2;
};

}