/**
 *  @author    Dubsky Tomas
 */
#include <print>
#include <stdexcept>

#include <ImGui/imgui.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include <RealEngine/utility/BuildType.hpp>
#include <RealEngine/utility/Error.hpp>
#include <RealEngine/window/WindowSubsystems.hpp>

namespace re {

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
    ImGui::GetIO().IniFilename = nullptr;
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
#if RE_BUILDING_FOR_DEBUG
        SDL_version compiled;
        SDL_VERSION(&compiled);
        std::println(
            "SDL compiled: {}.{}.{}", compiled.major, compiled.minor, compiled.patch
        );
#endif // RE_BUILDING_FOR_DEBUG
        SDL_version linked;
        SDL_GetVersion(&linked);
        std::println(
            "SDL linked:   {}.{}.{}", linked.major, linked.minor, linked.patch
        );
    }

    { // SDL2_ttf
#if RE_BUILDING_FOR_DEBUG
        SDL_version compiled;
        SDL_TTF_VERSION(&compiled);
        std::println(
            "TTF compiled: {}.{}.{}", compiled.major, compiled.minor, compiled.patch
        );
#endif // RE_BUILDING_FOR_DEBUG
        const SDL_version* linked = TTF_Linked_Version();
        std::println(
            "TTF linked:   {}.{}.{}", linked->major, linked->minor, linked->patch
        );
    }

    // ImGui
#if RE_BUILDING_FOR_DEBUG
    std::println("ImGui:        {}", ImGui::GetVersion());
#endif // RE_BUILDING_FOR_DEBUG
}

} // namespace re
