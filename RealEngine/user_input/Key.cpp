﻿/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/user_input/Key.hpp>

#include <SDL2/SDL_mouse.h>

namespace re {

Key SDLKToREKey(SDL_Keycode key) {
    switch (key) {
    case SDLK_BACKSPACE:    return Key::Backspace;
    case SDLK_TAB:          return Key::Tab;
    case SDLK_RETURN:       return Key::Return;
    case SDLK_PAUSE:        return Key::Pause;
    case SDLK_ESCAPE:       return Key::Escape;
    case SDLK_SPACE:        return Key::Space;
    case SDLK_COMMA:        return Key::Comma;
    case SDLK_MINUS:        return Key::Minus;
    case SDLK_PERIOD:       return Key::Period;
    case SDLK_SLASH:        return Key::Slash;
    case SDLK_0:            return Key::K0;
    case SDLK_1:            return Key::K1;
    case SDLK_2:            return Key::K2;
    case SDLK_3:            return Key::K3;
    case SDLK_4:            return Key::K4;
    case SDLK_5:            return Key::K5;
    case SDLK_6:            return Key::K6;
    case SDLK_7:            return Key::K7;
    case SDLK_8:            return Key::K8;
    case SDLK_9:            return Key::K9;
    case SDLK_COLON:        return Key::Colon;
    case SDLK_SEMICOLON:    return Key::Semicolon;
    case SDLK_EQUALS:       return Key::Equals;
    case SDLK_LEFTBRACKET:  return Key::LeftBracket;
    case SDLK_BACKSLASH:    return Key::Backslash;
    case SDLK_RIGHTBRACKET: return Key::RightBracket;
    case SDLK_a:            return Key::A;
    case SDLK_b:            return Key::B;
    case SDLK_c:            return Key::C;
    case SDLK_d:            return Key::D;
    case SDLK_e:            return Key::E;
    case SDLK_f:            return Key::F;
    case SDLK_g:            return Key::G;
    case SDLK_h:            return Key::H;
    case SDLK_i:            return Key::I;
    case SDLK_j:            return Key::J;
    case SDLK_k:            return Key::K;
    case SDLK_l:            return Key::L;
    case SDLK_m:            return Key::M;
    case SDLK_n:            return Key::N;
    case SDLK_o:            return Key::O;
    case SDLK_p:            return Key::P;
    case SDLK_q:            return Key::Q;
    case SDLK_r:            return Key::R;
    case SDLK_s:            return Key::S;
    case SDLK_t:            return Key::T;
    case SDLK_u:            return Key::U;
    case SDLK_v:            return Key::V;
    case SDLK_w:            return Key::W;
    case SDLK_x:            return Key::X;
    case SDLK_y:            return Key::Y;
    case SDLK_z:            return Key::Z;
    case SDLK_DELETE:       return Key::Delete;
    case SDLK_KP_0:         return Key::Numpad0;
    case SDLK_KP_1:         return Key::Numpad1;
    case SDLK_KP_2:         return Key::Numpad2;
    case SDLK_KP_3:         return Key::Numpad3;
    case SDLK_KP_4:         return Key::Numpad4;
    case SDLK_KP_5:         return Key::Numpad5;
    case SDLK_KP_6:         return Key::Numpad6;
    case SDLK_KP_7:         return Key::Numpad7;
    case SDLK_KP_8:         return Key::Numpad8;
    case SDLK_KP_9:         return Key::Numpad9;
    case SDLK_KP_PERIOD:    return Key::Decimal;
    case SDLK_KP_DIVIDE:    return Key::Divide;
    case SDLK_KP_MULTIPLY:  return Key::Multiply;
    case SDLK_KP_MINUS:     return Key::Subtract;
    case SDLK_KP_PLUS:      return Key::Add;
    case SDLK_KP_ENTER:     return Key::NumpadEnter;
    case SDLK_KP_EQUALS:    return Key::NumpadEquals;
    case SDLK_UP:           return Key::ArrowUp;
    case SDLK_DOWN:         return Key::ArrowDown;
    case SDLK_RIGHT:        return Key::ArrowRight;
    case SDLK_LEFT:         return Key::ArrowLeft;
    case SDLK_INSERT:       return Key::Insert;
    case SDLK_HOME:         return Key::Home;
    case SDLK_END:          return Key::End;
    case SDLK_PAGEUP:       return Key::PageUp;
    case SDLK_PAGEDOWN:     return Key::PageDown;
    case SDLK_F1:           return Key::F1;
    case SDLK_F2:           return Key::F2;
    case SDLK_F3:           return Key::F3;
    case SDLK_F4:           return Key::F4;
    case SDLK_F5:           return Key::F5;
    case SDLK_F6:           return Key::F6;
    case SDLK_F7:           return Key::F7;
    case SDLK_F8:           return Key::F8;
    case SDLK_F9:           return Key::F9;
    case SDLK_F10:          return Key::F10;
    case SDLK_F11:          return Key::F11;
    case SDLK_F12:          return Key::F12;
    case SDLK_F13:          return Key::F13;
    case SDLK_F14:          return Key::F14;
    case SDLK_F15:          return Key::F15;
    case SDLK_NUMLOCKCLEAR: return Key::NumLock;
    case SDLK_SCROLLLOCK:   return Key::ScrollLock;
    case SDLK_RSHIFT:       return Key::RShift;
    case SDLK_LSHIFT:       return Key::LShift;
    case SDLK_RCTRL:        return Key::RCtrl;
    case SDLK_LCTRL:        return Key::LCtrl;
    case SDLK_RALT:         return Key::RAlt;
    case SDLK_LALT:         return Key::LAlt;
    case SDLK_SYSREQ:       return Key::SysRq;
    case SDLK_MENU:         return Key::Menu;
    case SDLK_POWER:        return Key::Power;
    case SDL_BUTTON_LEFT:   return Key::LMB;
    case SDL_BUTTON_MIDDLE: return Key::MMB;
    case SDL_BUTTON_RIGHT:  return Key::RMB;
    case SDL_BUTTON_X1:     return Key::X1MB;
    case SDL_BUTTON_X2:     return Key::X2MB;
    default:                return Key::UnknownKey;
    }
}

std::string_view keyToString(Key key) {
    switch (key) {
    case Key::NoKey:       return "NoKey";
    case Key::AnyKey:      return "AnyKey";
    case Key::KeyUnbound:  return "KeyUnbound";
    case Key::Backspace:   return "Backspace";
    case Key::Tab:         return "Tab";
    case Key::Return:      return "Return";
    case Key::Pause:       return "Pause";
    case Key::Escape:      return "Escape";
    case Key::Space:       return "Space";
    case Key::Comma:       return "Comma";
    case Key::Minus:       return "Minus";
    case Key::Period:      return "Period";
    case Key::Slash:       return "Slash";
    case Key::K0:          return "K0";
    case Key::K1:          return "K1";
    case Key::K2:          return "K2";
    case Key::K3:          return "K3";
    case Key::K4:          return "K4";
    case Key::K5:          return "K5";
    case Key::K6:          return "K6";
    case Key::K7:          return "K7";
    case Key::K8:          return "K8";
    case Key::K9:          return "K9";
    case Key::Colon:       return "Colon";
    case Key::Semicolon:   return "Semicolon";
    case Key::Equals:      return "Equals";
    case Key::LeftBracket: return "LeftBracket";
    case Key::Backslash:   return "Backslash";
    case Key::RightBracket:return "RightBracket";
    case Key::A:           return "A";
    case Key::B:           return "B";
    case Key::C:           return "C";
    case Key::D:           return "D";
    case Key::E:           return "E";
    case Key::F:           return "F";
    case Key::G:           return "G";
    case Key::H:           return "H";
    case Key::I:           return "I";
    case Key::J:           return "J";
    case Key::K:           return "K";
    case Key::L:           return "L";
    case Key::M:           return "M";
    case Key::N:           return "N";
    case Key::O:           return "O";
    case Key::P:           return "P";
    case Key::Q:           return "Q";
    case Key::R:           return "R";
    case Key::S:           return "S";
    case Key::T:           return "T";
    case Key::U:           return "U";
    case Key::V:           return "V";
    case Key::W:           return "W";
    case Key::X:           return "X";
    case Key::Y:           return "Y";
    case Key::Z:           return "Z";
    case Key::Delete:      return "Delete";
    case Key::Numpad0:     return "Numpad0";
    case Key::Numpad1:     return "Numpad1";
    case Key::Numpad2:     return "Numpad2";
    case Key::Numpad3:     return "Numpad3";
    case Key::Numpad4:     return "Numpad4";
    case Key::Numpad5:     return "Numpad5";
    case Key::Numpad6:     return "Numpad6";
    case Key::Numpad7:     return "Numpad7";
    case Key::Numpad8:     return "Numpad8";
    case Key::Numpad9:     return "Numpad9";
    case Key::Decimal:     return "Decimal";
    case Key::Divide:      return "Divide";
    case Key::Multiply:    return "Multiply";
    case Key::Subtract:    return "Subtract";
    case Key::Add:         return "Add";
    case Key::NumpadEnter: return "NumpadEnter";
    case Key::NumpadEquals:return "NumpadEquals";
    case Key::ArrowUp:     return "ArrowUp";
    case Key::ArrowDown:   return "ArrowDown";
    case Key::ArrowRight:  return "ArrowRight";
    case Key::ArrowLeft:   return "ArrowLeft";
    case Key::Insert:      return "Insert";
    case Key::Home:        return "Home";
    case Key::End:         return "End";
    case Key::PageUp:      return "PageUp";
    case Key::PageDown:    return "PageDown";
    case Key::F1:          return "F1";
    case Key::F2:          return "F2";
    case Key::F3:          return "F3";
    case Key::F4:          return "F4";
    case Key::F5:          return "F5";
    case Key::F6:          return "F6";
    case Key::F7:          return "F7";
    case Key::F8:          return "F8";
    case Key::F9:          return "F9";
    case Key::F10:         return "F10";
    case Key::F11:         return "F11";
    case Key::F12:         return "F12";
    case Key::F13:         return "F13";
    case Key::F14:         return "F14";
    case Key::F15:         return "F15";
    case Key::NumLock:     return "NumLock";
    case Key::ScrollLock:  return "ScrollLock";
    case Key::RShift:      return "RShift";
    case Key::LShift:      return "LShift";
    case Key::RCtrl:       return "RCtrl";
    case Key::LCtrl:       return "LCtrl";
    case Key::RAlt:        return "RAlt";
    case Key::LAlt:        return "LAlt";
    case Key::SysRq:       return "SysRq";
    case Key::Menu:        return "Menu";
    case Key::Power:       return "Power";
    case Key::LMB:         return "LMB";
    case Key::MMB:         return "MMB";
    case Key::RMB:         return "RMB";
    case Key::X1MB:        return "X1MB";
    case Key::X2MB:        return "X2MB";
    case Key::UMW:         return "UMW";
    case Key::DMW:         return "DMW";
    case Key::LMW:         return "LMW";
    case Key::RMW:         return "RMW";
    default:               return "UnknownKey";
    }
}

uint64_t constexpr mix(char m, uint64_t s) {
    return ((s << 7) + ~(s >> 3)) + ~m;
}

uint64_t constexpr hash(const char* m) {
    return (*m) ? mix(*m, hash(m + 1)) : 0;
}

Key stringToKey(const std::string& string) {
    switch (hash(string.c_str())) {
    case hash("NoKey"):      return Key::NoKey;
    case hash("AnyKey"):     return Key::AnyKey;
    case hash("KeyUnbound"): return Key::KeyUnbound;
    case hash("Backspace"):   return Key::Backspace;
    case hash("Tab"):         return Key::Tab;
    case hash("Return"):      return Key::Return;
    case hash("Pause"):       return Key::Pause;
    case hash("Escape"):      return Key::Escape;
    case hash("Space"):       return Key::Space;
    case hash("Comma"):       return Key::Comma;
    case hash("Minus"):       return Key::Minus;
    case hash("Period"):      return Key::Period;
    case hash("Slash"):       return Key::Slash;
    case hash("K0"):          return Key::K0;
    case hash("K1"):          return Key::K1;
    case hash("K2"):          return Key::K2;
    case hash("K3"):          return Key::K3;
    case hash("K4"):          return Key::K4;
    case hash("K5"):          return Key::K5;
    case hash("K6"):          return Key::K6;
    case hash("K7"):          return Key::K7;
    case hash("K8"):          return Key::K8;
    case hash("K9"):          return Key::K9;
    case hash("Colon"):       return Key::Colon;
    case hash("Semicolon"):   return Key::Semicolon;
    case hash("Equals"):      return Key::Equals;
    case hash("LeftBracket"): return Key::LeftBracket;
    case hash("Backslash"):   return Key::Backslash;
    case hash("RightBracket"):return Key::RightBracket;
    case hash("A"):           return Key::A;
    case hash("B"):           return Key::B;
    case hash("C"):           return Key::C;
    case hash("D"):           return Key::D;
    case hash("E"):           return Key::E;
    case hash("F"):           return Key::F;
    case hash("G"):           return Key::G;
    case hash("H"):           return Key::H;
    case hash("I"):           return Key::I;
    case hash("J"):           return Key::J;
    case hash("K"):           return Key::K;
    case hash("L"):           return Key::L;
    case hash("M"):           return Key::M;
    case hash("N"):           return Key::N;
    case hash("O"):           return Key::O;
    case hash("P"):           return Key::P;
    case hash("Q"):           return Key::Q;
    case hash("R"):           return Key::R;
    case hash("S"):           return Key::S;
    case hash("T"):           return Key::T;
    case hash("U"):           return Key::U;
    case hash("V"):           return Key::V;
    case hash("W"):           return Key::W;
    case hash("X"):           return Key::X;
    case hash("Y"):           return Key::Y;
    case hash("Z"):           return Key::Z;
    case hash("Delete"):      return Key::Delete;
    case hash("Numpad0"):     return Key::Numpad0;
    case hash("Numpad1"):     return Key::Numpad1;
    case hash("Numpad2"):     return Key::Numpad2;
    case hash("Numpad3"):     return Key::Numpad3;
    case hash("Numpad4"):     return Key::Numpad4;
    case hash("Numpad5"):     return Key::Numpad5;
    case hash("Numpad6"):     return Key::Numpad6;
    case hash("Numpad7"):     return Key::Numpad7;
    case hash("Numpad8"):     return Key::Numpad8;
    case hash("Numpad9"):     return Key::Numpad9;
    case hash("Decimal"):     return Key::Decimal;
    case hash("Divide"):      return Key::Divide;
    case hash("Multiply"):    return Key::Multiply;
    case hash("Subtract"):    return Key::Subtract;
    case hash("Add"):         return Key::Add;
    case hash("NumpadEnter"): return Key::NumpadEnter;
    case hash("NumpadEquals"):return Key::NumpadEquals;
    case hash("ArrowUp"):     return Key::ArrowUp;
    case hash("ArrowDown"):   return Key::ArrowDown;
    case hash("ArrowRight"):  return Key::ArrowRight;
    case hash("ArrowLeft"):   return Key::ArrowLeft;
    case hash("Insert"):      return Key::Insert;
    case hash("Home"):        return Key::Home;
    case hash("End"):         return Key::End;
    case hash("PageUp"):      return Key::PageUp;
    case hash("PageDown"):    return Key::PageDown;
    case hash("F1"):          return Key::F1;
    case hash("F2"):          return Key::F2;
    case hash("F3"):          return Key::F3;
    case hash("F4"):          return Key::F4;
    case hash("F5"):          return Key::F5;
    case hash("F6"):          return Key::F6;
    case hash("F7"):          return Key::F7;
    case hash("F8"):          return Key::F8;
    case hash("F9"):          return Key::F9;
    case hash("F10"):         return Key::F10;
    case hash("F11"):         return Key::F11;
    case hash("F12"):         return Key::F12;
    case hash("F13"):         return Key::F13;
    case hash("F14"):         return Key::F14;
    case hash("F15"):         return Key::F15;
    case hash("NumLock"):     return Key::NumLock;
    case hash("ScrollLock"):  return Key::ScrollLock;
    case hash("RShift"):      return Key::RShift;
    case hash("LShift"):      return Key::LShift;
    case hash("RCtrl"):       return Key::RCtrl;
    case hash("LCtrl"):       return Key::LCtrl;
    case hash("RAlt"):        return Key::RAlt;
    case hash("LAlt"):        return Key::LAlt;
    case hash("SysRq"):       return Key::SysRq;
    case hash("Menu"):        return Key::Menu;
    case hash("Power"):       return Key::Power;
    case hash("LMB"):         return Key::LMB;
    case hash("MMB"):         return Key::MMB;
    case hash("RMB"):         return Key::RMB;
    case hash("X1MB"):        return Key::X1MB;
    case hash("X2MB"):        return Key::X2MB;
    case hash("UMW"):         return Key::UMW;
    case hash("DMW"):         return Key::DMW;
    case hash("LMW"):         return Key::LMW;
    case hash("RMW"):         return Key::RMW;
    default:                  return Key::UnknownKey;
    }
}

}