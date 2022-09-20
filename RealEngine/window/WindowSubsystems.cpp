/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/window/WindowSubsystems.hpp>

#include <stdexcept>

#include <SDL2/SDL.h>

#include <RealEngine/window/GL46Fixture.hpp>
#include <RealEngine/window/VK13Fixture.hpp>
#include <RealEngine/utility/Error.hpp>

namespace RE {

std::string to_string(RendererID r) {
    switch (r) {
    case RendererID::OPENGL46:      return "OPENGL46";
    case RendererID::VULKAN13:      return "VULKAN13";
    case RendererID::ANY:           return "ANY";
    default:                        return "Unknown renderer";
    }
}

void WindowSubsystems::initializeRenderer() const {
    switch (m_renderer) {
    case RendererID::OPENGL46: GL46Fixture::initialize(); break;
    }
}

WindowSubsystems::WindowSubsystems(RendererID preferredRenderer) :
    m_sdl2(), m_renderer(RendererID::ANY) {

    switch (preferredRenderer) {
    case RendererID::OPENGL46:  if (GL46Fixture::prepare()) { m_renderer = RendererID::OPENGL46; } break;
    case RendererID::VULKAN13:  if (VK13Fixture::prepare()) { m_renderer = RendererID::VULKAN13; } break;
    case RendererID::ANY:       if (VK13Fixture::prepare()) { m_renderer = RendererID::VULKAN13; } break;
    }

    if (m_renderer == RendererID::ANY) {//If the preferred renderer could not be prepared
        //Try to prepare all other renderers
        if (GL46Fixture::prepare()) { m_renderer = RendererID::OPENGL46; }
        if (VK13Fixture::prepare()) { m_renderer = RendererID::VULKAN13; }

        if (m_renderer == RendererID::ANY) {//If no renderer could be prepared
            //Throw becasue there is nothing more we can do
            throw std::runtime_error{"No renderer could be prepared!"};
        }
    }

    log(getVersion());
}

WindowSubsystems::SDL2_RAII::SDL2_RAII() {
    if (auto err = SDL_Init(SDL_INIT_EVERYTHING)) {
        const char* errorStr = SDL_GetError();
        error(errorStr);
        throw std::runtime_error{errorStr};
    }
    printVersion();
}

WindowSubsystems::SDL2_RAII::~SDL2_RAII() {
    SDL_Quit();
}

void WindowSubsystems::SDL2_RAII::printVersion() {
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