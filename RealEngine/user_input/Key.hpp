/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>

#include <SDL2/SDL_keycode.h>

namespace RE {

/**
 * @brief Every input (mouse/keyboard) key that can used.
*/
enum class Key {
	NO_KEY,
	ANY_KEY,
	KEY_UNBOUND,
	Backspace,
	Tab,
	Return,
	Pause,
	Escape,
	Space,
	Comma,
	Minus,
	Period,
	Slash,
	//Numbers above normal letters
	K0,
	K1,
	K2,
	K3,
	K4,
	K5,
	K6,
	K7,
	K8,
	K9,
	Colon,
	Semicolon,
	Equals,
	LeftBracket,
	Backslash,
	RightBracket,
	//Letters
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	Delete,
	//Numpad
	Numpad0,
	Numpad1,
	Numpad2,
	Numpad3,
	Numpad4,
	Numpad5,
	Numpad6,
	Numpad7,
	Numpad8,
	Numpad9,
	Decimal,
	Divide,
	Multiply,
	Subtract,
	Add,
	NumpadEnter,
	NumpadEquals,
	ArrowUp,
	ArrowDown,
	ArrowRight,
	ArrowLeft,
	Insert,
	Home,
	End,
	PageUp,
	PageDown,
	//Function keys
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	F13,
	F14,
	F15,
	NumLock,
	ScrollLock,
	RShift,
	LShift,
	RCtrl,
	LCtrl,
	RAlt,
	LAlt,
	SysRq,
	Menu,
	Power,
	//Mouse buttons
	LMB,
	MMB,
	RMB,
	X1MB,
	X2MB,
	UMW,
	DMW,
	LMW,
	RMW,

	UNKNOWN
};

Key SDLKToREKey(SDL_Keycode key);

std::string_view keyToString(Key key);

Key stringToKey(const std::string& string);

}