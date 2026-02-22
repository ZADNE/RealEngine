/**
 *  @author    Dubsky Tomas
 */
#include <RealEngine/window/WindowSubsystems.hpp>

#include <print>
#include <stdexcept>

#include <ImGui/imgui.h>
#include <SDL3/SDL_init.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <RealEngine/utility/BuildType.hpp>
#include <RealEngine/utility/Error.hpp>

namespace re {

WindowSubsystems::WindowSubsystems() {
    // SDL2
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        const char* errorStr = SDL_GetError();
        error(errorStr);
        throw std::runtime_error{errorStr};
    }

    // SDL2_ttf
    if (!TTF_Init()) {
        const char* errorStr = SDL_GetError();
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
    { // SDL3
#if RE_BUILDING_FOR_DEBUG
        std::println(
            "SDL compiled: {}.{}.{}", SDL_MAJOR_VERSION, SDL_MINOR_VERSION,
            SDL_MICRO_VERSION
        );
#endif // RE_BUILDING_FOR_DEBUG
        auto linked = SDL_GetVersion();
        std::println(
            "SDL linked:   {}.{}.{}", SDL_VERSIONNUM_MAJOR(linked),
            SDL_VERSIONNUM_MINOR(linked), SDL_VERSIONNUM_MICRO(linked)
        );
    }

    { // SDL3_ttf
#if RE_BUILDING_FOR_DEBUG
        std::println(
            "TTF compiled: {}.{}.{}", SDL_TTF_MAJOR_VERSION,
            SDL_TTF_MINOR_VERSION, SDL_TTF_MICRO_VERSION
        );
#endif // RE_BUILDING_FOR_DEBUG
        auto linked = TTF_Version();
        std::println(
            "TTF linked:   {}.{}.{}", SDL_VERSIONNUM_MAJOR(linked),
            SDL_VERSIONNUM_MINOR(linked), SDL_VERSIONNUM_MICRO(linked)
        );
    }

    // ImGui
#if RE_BUILDING_FOR_DEBUG
    std::println("ImGui:        {}", ImGui::GetVersion());
#endif // RE_BUILDING_FOR_DEBUG
}

} // namespace re
