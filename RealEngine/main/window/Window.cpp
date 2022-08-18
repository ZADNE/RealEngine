/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/main/window/Window.hpp>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_sdl.h>
#include <ImGui/imgui_impl_opengl3.h>

#include <RealEngine/utility/error.hpp>
#include <RealEngine/rendering/output/Viewport.hpp>

namespace RE {

void Window::goFullscreen(bool fullscreen, bool save) {
	m_flags.fullscreen = fullscreen;
	SDL_SetWindowFullscreen(m_SDLwindow, (fullscreen) ? SDL_WINDOW_FULLSCREEN : 0);
	SDL_GetWindowSize(m_SDLwindow, &m_dims.x, &m_dims.y);
	if (save) this->save();
}

void Window::goBorderless(bool borderless, bool save) {
	m_flags.borderless = borderless;
	SDL_SetWindowBordered(m_SDLwindow, (borderless) ? SDL_FALSE : SDL_TRUE);
	if (save) this->save();
}

void Window::setVSync(bool vSync, bool save) {
	m_flags.vSync = vSync;
	if (m_flags.vSync) {
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
	WindowSettings(settings), m_subsystems(settings.getRenderer()), m_windowTitle(title) {
	//Prepare flags
	Uint32 SDL_flags = SDL_WINDOW_OPENGL;
	if (m_flags.invisible)
		SDL_flags |= SDL_WINDOW_HIDDEN;
	if (m_flags.fullscreen)
		SDL_flags |= SDL_WINDOW_FULLSCREEN;
	if (m_flags.borderless)
		SDL_flags |= SDL_WINDOW_BORDERLESS;

	//Create window
	m_SDLwindow = SDL_CreateWindow(m_windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_dims.x, m_dims.y, SDL_flags);
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

	m_subsystems.initializeRenderer(getRenderer());

	//Set vertical synchronisation
	setVSync(m_flags.vSync, false);

	//Create window matrix uniform buffer
	Viewport::s_windowMatrix = glm::ortho(0.0f, static_cast<float>(m_dims.x), 0.0f, static_cast<float>(m_dims.y));
	Viewport::s_windowSize = m_dims;
	Viewport::s_windowMatrixUniformBuffer.emplace(UNIF_BUF_VIEWPORT_MATRIX,
		static_cast<int>(sizeof(Viewport::s_windowMatrix)), BufferUsageFlags::DYNAMIC_STORAGE, &Viewport::s_windowMatrix);

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

	//Destroy the viewport's buffer (before its implementation is null)
	Viewport::s_windowMatrixUniformBuffer.reset();
}

void Window::resize(const glm::ivec2& newDims, bool save) {
	SDL_SetWindowSize(m_SDLwindow, newDims.x, newDims.y);
	SDL_SetWindowPosition(m_SDLwindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_GetWindowSize(m_SDLwindow, &m_dims.x, &m_dims.y);

	Viewport::s_windowMatrix = glm::ortho(0.0f, static_cast<float>(m_dims.x), 0.0f, static_cast<float>(m_dims.y));
	Viewport::s_windowSize = m_dims;
	Viewport::setToWholeWindow();
	Viewport::setWindowMatrixToMatchViewport();

	if (save) this->save();
}

void Window::swapBuffer() {
	SDL_GL_SwapWindow(m_SDLwindow);
}

}