/*! 
 *  \author    Dubsky Tomas
 */
#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <array>

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_thread.h>
#include <nlohmann/json.hpp>

#include <magic_enum/magic_enum.hpp>
#include <RealEngine/user_input/Key.hpp>
#include <RealEngine/main/Error.hpp>
#include <RealEngine/main/MainProgram.hpp>

namespace RE {

template<typename KeyBindings>
using KeyBindingValueList = std::array<Key, magic_enum::enum_count<KeyBindings>()>;

/**
 * @brief Provides a mechanism for user-changeable key bindings
 *
 * Keybinder holds compile-time list of key bindings. Upon creation, the key bindings
 * are assigned previously saved values (= bound keys). If the saved bindings cannot be loaded,
 * default bindings are assigned.
 *
 * @tparam KeyBindings Enum class that contains the binding points. The values must be continous sequence 0..N-1
 * @tparam defaultValuesList List with default values of the bindings. These are used when saved bindings cannot be loaded.
*/
template<typename KeyBindings, const KeyBindingValueList<const KeyBindings>& defaultValuesList>
class KeyBinder {
	friend class MainProgram;
public:

	/**
	 * @brief Assignes bindings the previously saved values or the default ones.
	*/
	KeyBinder() {
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

	/**
	 * @brief Gets the currently bound key for the given key binding
	 * @param binding The binding to get the bound key of
	 * @return Currently bound key of the binding
	*/
	Key operator[](KeyBindings binding) const {
		return m_bindings[static_cast<size_t>(binding)];
	};

	/**
	 * @brief Immediately changes the binding to the given key and saves the change
	 * @param binding The binding to change
	 * @param key The key to assign
	*/
	void changeBinding(KeyBindings binding, Key key) {
		m_bindings[static_cast<size_t>(binding)] = key;
		saveCurrentBindings();
	};

	/**
	 * @brief Immediately resets the binding to its default key and saves the change
	 * @param binding The binding to change
	*/
	void resetBinding(KeyBindings binding) {
		changeBinding(binding, defaultValuesList[static_cast<size_t>(binding)]);
	};

	/**
	 * @brief Loads bindings from previously saved file
	 *
	 * If a binding cannot be loaded, its default value is assigned instead.
	 *
	 * @return True if all bindings could be loaded, false otherwise
	*/
	bool loadBindings() {
		nlohmann::json j;
		std::ifstream i(m_bindingFileName);
		i >> j;

		for (auto item = j.begin(); item != j.end(); item++) {
			auto binding = magic_enum::enum_cast<KeyBindings>(item.key());
			if (binding.has_value()) {
				m_bindings[static_cast<size_t>(*binding)] = stringToKey(item.value().get<std::string>());
			}
		}

		bool returnVal = false;
		for (size_t i = 0; i < magic_enum::enum_count<KeyBindings>(); i++) {
			if (m_bindings[i] == Key::NO_KEY) {
				m_bindings[i] = defaultValuesList[i];
				returnVal = true;
			}
		}
		return returnVal;//Indicates whether bindings have been up-to-date
	}

	/**
	 * @brief Resets all bindings to their default values
	 * @param permanently If true, the reset bindings are saved
	*/
	void resetBindings(bool permanently) {
		std::copy(defaultValuesList.begin(), defaultValuesList.end(), m_bindings.begin());
		if (permanently) { saveCurrentBindings(); }
	}

	/**
	 * @brief Saves current bindings
	*/
	void saveCurrentBindings() {
		nlohmann::ordered_json j;

		for (auto& enum_entry : magic_enum::enum_entries<KeyBindings>()) {
			j[std::string(enum_entry.second)] = keyToString(m_bindings[static_cast<size_t>(enum_entry.first)]);
		}

		std::ofstream o(m_bindingFileName, std::ofstream::trunc);
		o << j.dump(2);
		o.close();
	}

	/**
	 * @brief Asynchronously changes a keybinding
	 * @param binding The binding to change
	 * @param stopKey The key that stops the listening without changing the keybind
	*/
	template<typename CallbackReceiver, void(CallbackReceiver::* callback)(Key)>
	void listenChangeBinding(KeyBindings binding, CallbackReceiver& callbackReceiver, Key stopKey = Key::Delete) {
		auto info = new ListeningInfo<CallbackReceiver>{
			.binding = binding,
			.keyBinder = (*this),
			.stopKey = stopKey,
			.callbackReceiver = callbackReceiver
		};

		auto thread = SDL_CreateThread((listenForKey<CallbackReceiver, callback>), "keybind_listener", info);
		SDL_DetachThread(thread);
	};
private:
	template<typename CallbackReceiver>
	struct ListeningInfo {
		KeyBindings binding;
		KeyBinder& keyBinder;
		Key stopKey;
		CallbackReceiver& callbackReceiver;
	};

	template<typename CallbackReceiver, void (CallbackReceiver::* func)(Key)>
	static int listenForKey(void* ptr) {
		SDL_Event evnt;
		Key newKey = Key::UNKNOWN;
		MainProgram::std->checkForInput(false);
		ListeningInfo<CallbackReceiver>* info = reinterpret_cast<ListeningInfo<CallbackReceiver>*>(ptr);
		int rval = 0;

		while (newKey == Key::UNKNOWN) {//Until a key is pressed
			while (SDL_WaitEventTimeout(&evnt, 10)) {//Wait for events
				switch (evnt.type) {//Extract the pressed key (if it is one)
				case SDL_KEYDOWN:
					newKey = SDLKToREKey(evnt.key.keysym.sym);
					break;
				case SDL_MOUSEBUTTONDOWN:
					newKey = SDLKToREKey(evnt.button.button);
					break;
				case SDL_MOUSEWHEEL:
					if (evnt.wheel.y != 0) {
						newKey = (evnt.wheel.y > 0) ? Key::UMW : Key::DMW;
					} else {
						newKey = (evnt.wheel.x > 0) ? Key::RMW : Key::LMW;
					}
					break;
				}
			}
		}

		//If the pressed key is the one that stops the listening
		if (newKey == info->stopKey) {
			rval = 1;
		} else {//Successfully changed the binding
			info->keyBinder.changeBinding(info->binding, newKey);
		}

		//Callback
		(info->callbackReceiver.*func)(newKey);

		MainProgram::std->checkForInput(true);
		delete info;
		return rval;
	}

	std::string m_bindingFileName = "bindings.json";

	KeyBindingValueList<KeyBindings> m_bindings;
};

}