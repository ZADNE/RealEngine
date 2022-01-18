#pragma once
#include <string>

#include <SDL2/SDL_video.h>
#include <glm/vec2.hpp>

namespace RE {

	struct WindowFlags {
		WindowFlags(): invisible(false), fullscreen(false), borderless(false), vSync(false){}

		unsigned char invisible : 1, fullscreen : 1, borderless : 1, vSync : 1;
	};

	class Window {
	public:
		Window();
		~Window();

		int createWindow(const std::string& windowTitle, const glm::ivec2& dims, WindowFlags flags);

		//Editing functions
		void resizeWindow(const glm::ivec2& newDims);
		void goFullscreen(bool fullscreen);
		void goBorderless(bool borderless);
		void setVSync(bool vSync);

		//Title
		void setTitle(const std::string& title);
		std::string getTitle() const;


		void swapBuffer();

		glm::ivec2 getDims() const { return m_dims; }
		WindowFlags getFlags() const { return m_flags; }
	private:
		SDL_Window* m_SDLwindow = nullptr;
		SDL_GLContext m_SDL_GLContext = nullptr;
		WindowFlags m_flags;
		std::string m_windowTitle;

		glm::ivec2 m_dims = glm::ivec2(0, 0);
	};

}
