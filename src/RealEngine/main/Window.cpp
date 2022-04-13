#include <RealEngine/main/Window.hpp>

#include <GL/glew.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_sdl.h>
#include <ImGui/imgui_impl_opengl3.h>

#include <RealEngine/main/Error.hpp>
#include <RealEngine/graphics/Viewport.hpp>

namespace RE {

void Window::goFullscreen(bool fullscreen, bool save) {
	p_flags.fullscreen = fullscreen;
	SDL_SetWindowFullscreen(m_SDLwindow, (fullscreen) ? SDL_WINDOW_FULLSCREEN : 0);
	SDL_GetWindowSize(m_SDLwindow, &p_dims.x, &p_dims.y);
	if (save) this->save();
}

void Window::goBorderless(bool borderless, bool save) {
	p_flags.borderless = borderless;
	SDL_SetWindowBordered(m_SDLwindow, (borderless) ? SDL_FALSE : SDL_TRUE);
	if (save) this->save();
}

void Window::setVSync(bool vSync, bool save) {
	p_flags.vSync = vSync;
	if (p_flags.vSync) {
		if (SDL_GL_SetSwapInterval(-1)) {
			//Cannot use adaptive vSync, use regular vSync
			error(SDL_GetError());
			SDL_GL_SetSwapInterval(1);
		}
	} else {
		SDL_GL_SetSwapInterval(0);
	}
	if (save) this->save();
}

void Window::setTitle(const std::string& title) {
	m_windowTitle = title;
	SDL_SetWindowTitle(m_SDLwindow, title.c_str());
}

const std::string& Window::getTitle() const {
	return m_windowTitle;
}

Window::Window(const WindowSettings& settings, const std::string& title) :
	WindowSettings(settings), m_windowTitle(title) {
	//Prepare flags
	Uint32 SDL_flags = SDL_WINDOW_OPENGL;
	if (p_flags.invisible)
		SDL_flags |= SDL_WINDOW_HIDDEN;
	if (p_flags.fullscreen)
		SDL_flags |= SDL_WINDOW_FULLSCREEN;
	if (p_flags.borderless)
		SDL_flags |= SDL_WINDOW_BORDERLESS;

	//Create window
	m_SDLwindow = SDL_CreateWindow(m_windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, p_dims.x, p_dims.y, SDL_flags);
	if (!m_SDLwindow) {
		error(SDL_GetError());
		fatalError("Could not create window");
	}

	//Create OpenGL context for the window
	m_GLContext = SDL_GL_CreateContext(m_SDLwindow);
	if (!m_GLContext) {
		error(SDL_GetError());
		fatalError("Could not create OpenGL context");
	}

	//Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fatalError("GLEW failed initialization!");
	}

	//Print OpenGL info
	std::printf("OpenGL:       %s\n", glGetString(GL_VERSION));
	std::printf("GLSL:         %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	std::printf("Renderer:     %s\n", glGetString(GL_RENDERER));
	std::printf("Vendor:       %s\n", glGetString(GL_VENDOR));

	//Set vertical synchronisation
	setVSync(p_flags.vSync, false);

#ifdef _DEBUG
	//Enable OpenGL error callbacks
	if (glDebugMessageCallback) {
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(openglCallbackFunction, nullptr);
		GLuint ids[] = {
			131185 //HORDCODE INGORE: Buffer object video memory notification
		};
		glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, sizeof(ids) / sizeof(GLuint), ids, GL_FALSE);
	}
#endif // _DEBUG

	//Disable dither that is probably no-op anyway
	glDisable(GL_DITHER);

	//Do not use multisample by default
	glDisable(GL_MULTISAMPLE);

	//Use blenbing by default
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Byte alignment
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//Primitive restart index
	glEnable(GL_PRIMITIVE_RESTART);
	glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);

	//Create window matrix uniform buffer
	Viewport::m_windowMatrix = glm::ortho(0.0f, static_cast<float>(p_dims.x), 0.0f, static_cast<float>(p_dims.y));
	Viewport::m_windowSize = p_dims;
	Viewport::m_windowMatrixUniformBuffer.emplace(
		UNIF_BUF_VIEWPORT_MATRIX, true, sizeof(Viewport::m_windowMatrix), RE::BufferUsageFlags::DYNAMIC_STORAGE, &Viewport::m_windowMatrix);

	//Initialize ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForOpenGL(m_SDLwindow, m_GLContext);
	ImGui_ImplOpenGL3_Init("#version 460 core");
}

Window::~Window() {
	//Shutdown ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	//Delete OpenGL context
	SDL_GL_DeleteContext(m_GLContext);

	//Destroy the SDL window
	SDL_DestroyWindow(m_SDLwindow);
}

void Window::resize(const glm::ivec2& newDims, bool save) {
	SDL_SetWindowSize(m_SDLwindow, newDims.x, newDims.y);
	SDL_SetWindowPosition(m_SDLwindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_GetWindowSize(m_SDLwindow, &p_dims.x, &p_dims.y);

	Viewport::m_windowMatrix = glm::ortho(0.0f, static_cast<float>(p_dims.x), 0.0f, static_cast<float>(p_dims.y));
	Viewport::m_windowSize = p_dims;
	Viewport::setToWholeWindow();
	Viewport::setWindowMatrixToMatchViewport();

	if (save) this->save();
}

void Window::swapBuffer() {
	SDL_GL_SwapWindow(m_SDLwindow);
}

}