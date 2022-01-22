#include <RealEngine/graphics/CursorDrawer.hpp>

#include <SDL2/SDL_events.h>

namespace RE {

	SDL_Cursor* CursorDrawer::m_cursor = SDL_GetDefaultCursor();
	CURSOR_STATE CursorDrawer::m_currentState = CURSOR_STATE::ARROW;

	void CursorDrawer::setCursor(CURSOR_STATE state) {
		if (m_currentState == state) { return; }//Ignoring redundant calls
		if (state == CURSOR_STATE::INVISIBLE) {
			SDL_ShowCursor(SDL_FALSE);
		}
		else {
			if (m_currentState == CURSOR_STATE::INVISIBLE) {//Invisible was previous, we need to show the cursor again
				SDL_ShowCursor(SDL_TRUE);
			}
			SDL_FreeCursor(m_cursor);
			m_cursor = SDL_CreateSystemCursor((SDL_SystemCursor)state);
			SDL_SetCursor(m_cursor);
		}
		m_currentState = state;
	}

	CURSOR_STATE CursorDrawer::getCursor() {
		return m_currentState;
	}

}