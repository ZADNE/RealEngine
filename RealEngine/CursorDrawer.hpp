#pragma once
#include <SDL2/SDL_mouse.h>

namespace RE {

	enum class CURSOR_STATE {
		ARROW = SDL_SYSTEM_CURSOR_ARROW,     /**< Arrow */
		IBEAM = SDL_SYSTEM_CURSOR_IBEAM,     /**< I-beam */
		WAIT = SDL_SYSTEM_CURSOR_WAIT,      /**< Wait */
		CROSSHAIR = SDL_SYSTEM_CURSOR_CROSSHAIR, /**< Crosshair */
		WAITARROW = SDL_SYSTEM_CURSOR_WAITARROW, /**< Small wait cursor (or Wait if not available) */
		SIZENWSE = SDL_SYSTEM_CURSOR_SIZENWSE,  /**< Double arrow pointing northwest and southeast */
		SIZENESW = SDL_SYSTEM_CURSOR_SIZENESW,  /**< Double arrow pointing northeast and southwest */
		SIZEWE = SDL_SYSTEM_CURSOR_SIZEWE,    /**< Double arrow pointing west and east */
		SIZENS = SDL_SYSTEM_CURSOR_SIZENS,    /**< Double arrow pointing north and south */
		SIZEALL = SDL_SYSTEM_CURSOR_SIZEALL,   /**< Four pointed arrow pointing north, south, east, and west */
		NO = SDL_SYSTEM_CURSOR_NO,        /**< Slashed circle or crossbones */
		HAND = SDL_SYSTEM_CURSOR_HAND,      /**< Hand */
		INVISIBLE
	};

	class CursorDrawer {
	public:

		static void setCursor(CURSOR_STATE state);

		static CURSOR_STATE getCursor();
	private:
		static SDL_Cursor* m_cursor;
		static CURSOR_STATE m_currentState;
	};

}
