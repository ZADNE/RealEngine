/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>

/*! \mainpage RealEngine
 *
 * \section overview Overview
 *
 * To get a minimal working RealEngine application, you need to create your own
 * room class that inherits from Room. The typical main function then looks like
 * this: \code {cpp} int main(int argc, char* argv[]) {
 *      MainProgram::initialize();
 *      auto* myRoom = MainProgram::addRoom<MyDerivedRoom>(constructorArgs);
 *      return MainProgram::run(myRoom->name(), transitionparameters);
 *  }
 * \endcode
 */

namespace re {

constexpr int k_versionMajor = 2;
constexpr int k_versionMinor = 0;
constexpr int k_versionPatch = 0;

/**
 * @brief Is integer representation of RealEngine's version
 */
constexpr int k_version = k_versionMajor * 1'000'000 + k_versionMinor * 1'000 +
                          k_versionPatch;

/**
 * @brief Lists all renderers known to RealEngine
 *
 * The order also represents precedence when no specific renderer is
 * preffered (or if it fails during initialization).
 */
enum class RendererID {
    Vulkan13, /**< Vulkan 1.3 renderer */
    Any
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

private:
    /**
     * @brief Is a very simple RAII wrapper around SDL2
     */
    class SDL2_RAII {
    public:
        SDL2_RAII();

        SDL2_RAII(const SDL2_RAII&)            = delete; /**< Noncopyable */
        SDL2_RAII& operator=(const SDL2_RAII&) = delete; /**< Noncopyable */

        SDL2_RAII(SDL2_RAII&&)            = delete; /**< Nonmovable */
        SDL2_RAII& operator=(SDL2_RAII&&) = delete; /**< Nonmovable */

        ~SDL2_RAII();

        void printVersion() const;
    };

    SDL2_RAII m_sdl2;
};

} // namespace re