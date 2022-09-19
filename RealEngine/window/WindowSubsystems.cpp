/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/window/WindowSubsystems.hpp>

#include <SDL2/SDL.h>

#include <RealEngine/utility/Error.hpp>
#include <RealEngine/window/GL46Fixture.hpp>

namespace RE {

std::string to_string(RendererID r) {
    switch (r) {
    case RendererID::OPENGL_46: return "OPENGL_46";
    default: return "Unknown renderer";
    }
}

void printSDLVersion() {
#ifndef NDEBUG
    SDL_version compiled;
    SDL_VERSION(&compiled);
    std::printf("SDL compiled: %u.%u.%u\n", compiled.major, compiled.minor, compiled.patch);
#endif // DEBUG
    SDL_version linked;
    SDL_GetVersion(&linked);
    std::printf("SDL linked:   %u.%u.%u\n", linked.major, linked.minor, linked.patch);
}

void WindowSubsystems::initializeRenderer(RendererID renderer) const {
    switch (renderer) {
    case RendererID::OPENGL_46: GL46Fixture::initialize(); break;
    }
}

WindowSubsystems::WindowSubsystems(RendererID renderer) {
    log(getVersion());
    int err = 0;

    //SDL2
    if (err = SDL_Init(SDL_INIT_EVERYTHING)) {
        error(SDL_GetError());
        goto fail;
    }
    printSDLVersion();

    switch (renderer) {
    case RendererID::OPENGL_46: GL46Fixture::prepare(); break;
    default: goto quitSDL_fail;
    }

    return;//Successfully initialized

quitSDL_fail:
    SDL_Quit();
fail:
    throw err;
}

WindowSubsystems::~WindowSubsystems() {
    SDL_Quit();
}

}