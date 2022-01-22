#include <RealEngine/main/details/RealEngine.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <GL/glew.h>

#include <RealEngine/main/Error.hpp>

namespace RE {

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

void printTTFVersion() {
#ifdef _DEBUG
	SDL_version compiled;
	TTF_VERSION(&compiled);
	std::printf("TTF compiled: %u.%u.%u\n", compiled.major, compiled.minor, compiled.patch);
#endif // _DEBUG
	auto linked = TTF_Linked_Version();
	std::printf("TTF linked:   %u.%u.%u\n", linked->major, linked->minor, linked->patch);
}

RealEngine::RealEngine() {
	log(getVersion());
	int err;

	//SDL2
	if (err = SDL_Init(SDL_INIT_EVERYTHING)) {
		error(SDL_GetError());
		goto fail;
	}
	printSDLVersion();
	SDL_StopTextInput();

	//SDL2 TTF
	if (err = TTF_Init()) {
		error(TTF_GetError());
		goto quitSDL_fail;
	}
	printTTFVersion();

	if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)) {
		error("Cannot use doublebuffer!");
	}

#ifdef _DEBUG
	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG)) {
		error("Cannot use debug context!");
	}
#endif // _DEBUG

	if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE)) {
		error("Cannot use core profile context!");
	}

	return;//Successfully initialized

//quitTTF_quitSDL_fail:
	TTF_Quit();
quitSDL_fail:
	SDL_Quit();
fail:
	throw err;
}

RealEngine::~RealEngine() {
	TTF_Quit();
	SDL_Quit();
	log("RealEngine quit");
}

}