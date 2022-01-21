#include <RealEngine/main/Window.hpp>

#include <GL/glew.h>

#include <RealEngine/main/Error.hpp>

namespace RE {

	Window::Window() {

	}


	Window::~Window() {
		if (m_SDL_GLContext) {
			SDL_GL_DeleteContext(m_SDL_GLContext);
		}
		if (m_SDLwindow) {
			SDL_DestroyWindow(m_SDLwindow);
		}
	}

	int Window::createWindow(const std::string& windowTitle, const glm::ivec2& dims, WindowFlags flags) {
		Uint32 SDL_flag = SDL_WINDOW_OPENGL;
		m_windowTitle = windowTitle;

		if (flags.invisible) {
			SDL_flag |= SDL_WINDOW_HIDDEN;
		}

		if (flags.fullscreen) {
			SDL_flag |= SDL_WINDOW_FULLSCREEN;
		}

		if (flags.borderless) {
			SDL_flag |= SDL_WINDOW_BORDERLESS;
		}
		m_flags = flags;

#ifdef _DEBUG
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif // _DEBUG

		m_SDLwindow = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, dims.x, dims.y, SDL_flag);

		if (m_SDLwindow == nullptr) {
			fatalError("SDL window failed to open!");
		}
		m_dims = dims;

		m_SDL_GLContext = SDL_GL_CreateContext(m_SDLwindow);

		if (m_SDL_GLContext == nullptr) {
			fatalError("SDL_GL context could not be created!");
		}

		GLenum error = glewInit();
		if (error != GLEW_OK) {
			fatalError("GLEW failed initialization!");
		}
		std::printf("OpenGL version: %s \n", glGetString(GL_VERSION));
		std::printf("GLSL version: %s \n", glGetString(GL_SHADING_LANGUAGE_VERSION));
		std::printf("GPU: %s \n", glGetString(GL_RENDERER));


		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

#ifdef _DEBUG
		if (glDebugMessageCallback) {
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(openglCallbackFunction, nullptr);
			GLuint ids[] = {
				131185 //Buffer object video memory notification
			};
			glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, sizeof(ids) / sizeof(GLuint), ids, GL_FALSE);
		}
#endif // _DEBUG

		//V-SYNC
		SDL_GL_SetSwapInterval((m_flags.vSync) ? 1 : 0);

		glDisable(GL_DITHER);
		glDisable(GL_MULTISAMPLE);

		//Blendmode
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Byte alignment
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		return 0;
	}

	void Window::resizeWindow(const glm::ivec2& newDims) {
		SDL_SetWindowSize(m_SDLwindow, newDims.x, newDims.y);
		SDL_SetWindowPosition(m_SDLwindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		m_dims = newDims;
	}

	void Window::goFullscreen(bool fullscreen) {
		m_flags.fullscreen = fullscreen;
		SDL_SetWindowFullscreen(m_SDLwindow, (fullscreen) ? SDL_WINDOW_FULLSCREEN : 0);
	}

	void Window::goBorderless(bool borderless) {
		m_flags.borderless = borderless;
		SDL_SetWindowBordered(m_SDLwindow, (borderless) ? SDL_TRUE : SDL_FALSE);
	}

	void Window::setVSync(bool vSync) {
		m_flags.vSync = vSync;
		SDL_GL_SetSwapInterval((vSync) ? 1 : 0);
	}

	void Window::setTitle(const std::string& title) {
		m_windowTitle = title;
		SDL_SetWindowTitle(m_SDLwindow, title.c_str());
	}

	std::string Window::getTitle() const {
		return m_windowTitle;
	}

	void Window::swapBuffer() {
		SDL_GL_SwapWindow(m_SDLwindow);
	}

}