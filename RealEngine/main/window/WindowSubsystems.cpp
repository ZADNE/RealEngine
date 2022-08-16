/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/main/window/WindowSubsystems.hpp>

#include <SDL2/SDL.h>

#include <RealEngine/utility/error.hpp>
#include <RealEngine/rendering/internal_renderers/GL46_Renderer.hpp>

namespace RE {

std::string to_string(Renderer r) {
	switch (r) {
	case RE::Renderer::OPENGL_46: return "OPENGL_46";
	default: return "Unknown renderer";
	}
}

void printSDLVersion() {
#ifdef _DEBUG
	SDL_version compiled;
	SDL_VERSION(&compiled);
	std::printf("SDL compiled: %u.%u.%u\n", compiled.major, compiled.minor, compiled.patch);
#endif // _DEBUG
	SDL_version linked;
	SDL_GetVersion(&linked);
	std::printf("SDL linked:   %u.%u.%u\n", linked.major, linked.minor, linked.patch);
}

void WindowSubsystems::initializeRenderer(Renderer renderer) const {
	switch (renderer) {
	case RE::Renderer::OPENGL_46: GL46_Renderer::initialize(); break;
	}
}

WindowSubsystems::WindowSubsystems(Renderer renderer) {
	log(getVersion());
	int err = 0;

	//SDL2
	if (err = SDL_Init(SDL_INIT_EVERYTHING)) {
		error(SDL_GetError());
		goto fail;
	}
	printSDLVersion();

	switch (renderer) {
	case RE::Renderer::OPENGL_46: GL46_Renderer::prepare(); break;
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