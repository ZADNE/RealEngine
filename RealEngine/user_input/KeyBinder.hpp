#pragma once
/*
Example of binding list

#define KB_BINDING_LIST(m)   \
	m(VIEW, MOVE_LEFT)  \
	m(VIEW, MOVE_RIGHT)  \
	m(VIEW, MOVE_UP)  \
	m(VIEW, MOVE_DOWN)  \
	m(VIEW, ZOOM_IN)  \
	m(VIEW, ZOOM_OUT)  \
	m(BUILDMENU, OPEN)  \
	m(BUILDMENU, CHOOSE)  \
	m(DEBUG, ENDGAME)  \
	m(DEBUG, WORLDDRAW)
	  /|\		/|\
section name	binding name

*/


#ifdef KB_BINDING_LIST
#include <iostream>
#include <string>
#include <array>
#include <fstream>

#include <SDL2/SDL_thread.h>
#include <nlohmann/json.hpp>

#include <RealEngine/utility/utility.hpp>
#include <RealEngine/main/Error.hpp>
#include <RealEngine/user_input/Key.hpp>
#include <RealEngine/main/MainProgram.hpp>

//Deliberately outside of RE namespace
namespace {
SMARTENUM_INIT_ALL(BL, KB_BINDING_LIST)
}

#define KB(enum) (RE::KeyBinder::std())[BL::e##enum]

namespace RE {

class KeyBinder {
	friend class MainProgram;
public:

	KeyBinder() : KeyBinder(std::array<RE::Key, (size_t)BL::eBL_Count>{}) {

	}

	KeyBinder(std::array<RE::Key, (size_t)BL::eBL_Count> defBindings) : m_bindingsDefault(defBindings) {
		try {
			if (loadBindings()) {
				saveCurrentBindings();//Bindings are not latest-version, saving current one
				std::cout << "Bindings successfully updated.\n";
			} else {
				std::cout << "Loaded up-to-date bindings.\n";
			}
		}
		catch (...) {//No bindings exist, resetting them
			std::cout << "No bindings exist, creating default ones.\n";
			resetBindings(true);
		}
	}

	RE::Key operator[](BL binding) const {
		return m_bindings[binding];
	};

	//Immediately changes the binding to the given key 
	void changeBinding(BL binding, Key key) {
		m_bindings[binding] = key;
		saveCurrentBindings();
	};

	//Returns whether the binding were up-to-date
	bool loadBindings() {
		nlohmann::json j;
		std::ifstream i(m_bindingFileName);
		i >> j;

		for (auto outer = j.begin(); outer != j.end(); outer++) {
			for (auto inner = outer.value().begin(); inner != outer.value().end(); inner++) {
				m_bindings[(size_t)(stringToEnum(BL, 'e' + outer.key() + '_' + inner.key()))] = RE::stringToKey(inner.value().get<std::string>());
			}
		}

		bool returnVal = false;
		for (int i = 0; i < (int)BL::eBL_Count; i++) {
			if (m_bindings[i] == RE::Key::NO_KEY && i != (int)BL::eBL_Error) {
				m_bindings[i] = m_bindingsDefault[i];
				returnVal = true;
			}
		}
		return returnVal;//Indicates whether bindings have been up-to-date
	}

	void resetBindings(bool permanently) {
		std::copy(m_bindingsDefault.begin(), m_bindingsDefault.end(), m_bindings.begin());
		if (permanently) { saveCurrentBindings(); }
	}

	void saveCurrentBindings() {
		nlohmann::ordered_json j;

		std::string cat;

		for (size_t i = 0u; i < (size_t)BL::eBL_Error; ++i) {
			j[enumToCat(BL, i)][enumToIden(BL, i)] = RE::keyToString(m_bindings[i]);
		}
		std::ofstream o(m_bindingFileName, std::ofstream::trunc);
		o << j.dump(2);
		o.close();
	}

	static KeyBinder& std() {
	#ifdef KB_DEFAULT_LIST
		static KeyBinder std{KB_DEFAULT_LIST};
	#else
		static KeyBinder std{};
	#endif // KB_DEFAULT_LIST
		return std;
	};

	//Changes the binding to the first key that the user presses
	//The key is saved into newKey
	//Blocks normal key input until new key is pressed
	//Pressing the stopKey will stop the listening and either: unbind the binding or leave without any changes to the bindings (depedning on 'unbindKeyAfterStop')
	//All is done in side thread, main thread keeps going
	void listenChangeBinding(BL binding, Key* newKey, Key stopKey = Key::Escape, bool unbindKeyAfterStop = false) {
		m_listenBinding = binding;
		auto info = new ListeningInfo();
		info->KB = this;
		info->newKey = newKey;
		info->stopKey = stopKey;
		info->unbindKeyAfterStop = unbindKeyAfterStop;

		auto thread = SDL_CreateThread(listenFoKey, "temp_keybind_listener", info);
		SDL_DetachThread(thread);
	};
private:
	struct ListeningInfo {
		KeyBinder* KB;
		Key* newKey;
		Key stopKey;
		bool unbindKeyAfterStop;
	};

	static int listenFoKey(void* ptr) {
		SDL_Event evnt;
		Key key = Key::UNKNOWN;
		MainProgram::std->checkForInput(false);
		ListeningInfo* info = (ListeningInfo*)ptr;

		while (key == Key::UNKNOWN) {
			SDL_WaitEvent(&evnt);
			switch (evnt.type) {
			case SDL_KEYDOWN:
				key = SDLKToREKey(evnt.key.keysym.sym);
				if (key == info->stopKey) {
					stopListening(info);
					return 1;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				key = SDLKToREKey(evnt.button.button);
				if (key == info->stopKey) {
					stopListening(info);
					return 1;
				}
				break;
			case SDL_MOUSEWHEEL:
				if (evnt.wheel.y != 0) {
					key = (evnt.wheel.y > 0) ? (Key::UMW) : (Key::DMW);
					if (key == info->stopKey) {
						stopListening(info);
						return 1;
					}
				}
				key = (evnt.wheel.x > 0) ? (Key::RMW) : (Key::LMW);
				if (key == info->stopKey) {
					stopListening(info);
					return 1;
				}
				break;
			}
		}
		info->KB->changeBinding(info->KB->m_listenBinding, key);

		if (info->newKey) {//If the player wants to know (supplied) the newKey
			*info->newKey = key;
		}

		MainProgram::std->checkForInput(true);
		delete info;
		return 0;
	}

	static void stopListening(ListeningInfo* info) {
		MainProgram::std->checkForInput(true);
		if (info->unbindKeyAfterStop) {
			info->KB->changeBinding(info->KB->m_listenBinding, Key::KEY_UNBOUND);
		}
		if (info->newKey) {//If the player wants to know (supplied) the newKey
			*info->newKey = (info->unbindKeyAfterStop) ? Key::KEY_UNBOUND : Key::NO_KEY;
		}
		delete info;
	}


	void finishedListening(const RE::Key& newKey) {
		m_bindings[m_listenBinding] = newKey;
		saveCurrentBindings();
	};

	BL m_listenBinding = BL::eBL_Error;
	std::string m_bindingFileName = "bindings.json";

	std::array<Key, (size_t)BL::eBL_Count> m_bindings;
	std::array<Key, (size_t)BL::eBL_Count> m_bindingsDefault;
};

}
#endif // KB_BINDING_LIST