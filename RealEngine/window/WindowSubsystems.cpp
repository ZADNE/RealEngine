/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/window/WindowSubsystems.hpp>

#include <stdexcept>

#include <SDL2/SDL.h>
#include <ImGui/imgui.h>

#include <RealEngine/utility/Error.hpp>

namespace RE {

std::string to_string(RendererID r) {
    switch (r) {
    case RendererID::Vulkan13:      return "Vulkan13";
    case RendererID::Any:           return "Any";
    default:                        return "Unknown renderer";
    }
}

WindowSubsystems::WindowSubsystems() :
    m_sdl2() {
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
}

void WindowSubsystems::printRealEngineVersion() {
    log(RealEngineVersionString());
}

void WindowSubsystems::printSubsystemsVersions() const {
    m_sdl2.printVersion();
}

WindowSubsystems::SDL2_RAII::SDL2_RAII() {
    if (auto err = SDL_Init(SDL_INIT_EVERYTHING)) {
        const char* errorStr = SDL_GetError();
        error(errorStr);
        throw std::runtime_error{errorStr};
    }
}

WindowSubsystems::SDL2_RAII::~SDL2_RAII() {
    ImGui::DestroyContext();
    SDL_Quit();
}

void WindowSubsystems::SDL2_RAII::printVersion() const {
#ifndef NDEBUG
    SDL_version compiled;
    SDL_VERSION(&compiled);
    std::printf("SDL compiled: %u.%u.%u\n", compiled.major, compiled.minor, compiled.patch);
#endif // DEBUG
    SDL_version linked;
    SDL_GetVersion(&linked);
    std::printf("SDL linked:   %u.%u.%u\n", linked.major, linked.minor, linked.patch);
}

}