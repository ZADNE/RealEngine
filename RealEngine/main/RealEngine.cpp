#include <RealEngine/main/RealEngine.hpp>

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include <RealEngine/main/Error.hpp>

namespace RE {

	int initRE() {
		printf("RealEngine v%d.%d.%d\n", RE_VERSION_MAJOR, RE_VERSION_MINOR, RE_VERSION_PATCH);

		SDL_Init(SDL_INIT_EVERYTHING);

		SDL_version linked;
		SDL_GetVersion(&linked);

#ifdef _DEBUG
		SDL_version compiled;
		SDL_VERSION(&compiled);
		printf("SDL compiled: %u.%u.%u\nSDL linked: %u.%u.%u\n", compiled.major, compiled.minor, compiled.patch, linked.major, linked.minor, linked.patch);
#else
		printf("SDL version: %u.%u.%u\n", linked.major, linked.minor, linked.patch);
#endif // _DEBUG

		SDL_StopTextInput();

		if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1)) {
			fatalError("Failed to set SDL_GL_DOUBLEBUFFER!\n");
		}

		/*if (SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1)) {
			fatalError("Failed to set SDL_GL_ACCELERATED_VISUAL!\n");
		}*/

		return 0;
	}

}