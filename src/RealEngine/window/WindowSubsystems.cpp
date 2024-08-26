/*!
 *  @author    Dubsky Tomas
 */
#include <stdexcept>

#include <ImGui/imgui.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <RealEngine/utility/Error.hpp>
#include <RealEngine/window/WindowSubsystems.hpp>

namespace re {

std::string to_string(RendererID r) {
    switch (r) {
    case RendererID::Vulkan13: return "Vulkan13";
    case RendererID::Any:      return "Any";
    default:                   return "Unknown renderer";
    }
}

WindowSubsystems::WindowSubsystems() {
    // SDL2
    if (auto err = SDL_Init(SDL_INIT_EVERYTHING)) {
        const char* errorStr = SDL_GetError();
        error(errorStr);
        throw std::runtime_error{errorStr};
    }

    // SDL2_ttf
    if (TTF_Init() != 0) {
        const char* errorStr = TTF_GetError();
        error(errorStr);
        throw std::runtime_error{errorStr};
    }

    // ImGui
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
}

WindowSubsystems::~WindowSubsystems() {
    ImGui::DestroyContext();
    TTF_Quit();
    SDL_Quit();
}

void WindowSubsystems::printRealEngineVersion() {
    log(RealEngineVersionString());
}

void WindowSubsystems::printSubsystemsVersions() const {
    { // SDL2
#ifndef NDEBUG
        SDL_version compiled;
        SDL_VERSION(&compiled);
        std::printf(
            "SDL compiled: %u.%u.%u\n", compiled.major, compiled.minor, compiled.patch
        );
#endif // DEBUG
        SDL_version linked;
        SDL_GetVersion(&linked);
        std::printf(
            "SDL linked:   %u.%u.%u\n", linked.major, linked.minor, linked.patch
        );
    }

    { // SDL2_ttf
#ifndef NDEBUG
        SDL_version compiled;
        SDL_TTF_VERSION(&compiled);
        std::printf(
            "TTF compiled: %d.%d.%d\n", compiled.major, compiled.minor, compiled.patch
        );
#endif // DEBUG
        const SDL_version* linked = TTF_Linked_Version();
        std::printf(
            "TTF linked:   %d.%d.%d\n", linked->major, linked->minor, linked->patch
        );
    }

    // ImGui
#ifndef NDEBUG
    std::printf("ImGui:        %s\n", ImGui::GetVersion());
#endif // DEBUG
}

} // namespace re
