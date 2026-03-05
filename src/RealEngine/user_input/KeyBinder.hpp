/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <array>
#include <fstream>
#include <iostream>
#include <optional>
#include <span>
#include <string>
#include <utility>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_thread.h>

#include <RealEngine/program/MainProgram.hpp>
#include <RealEngine/user_input/Key.hpp>
#include <RealEngine/utility/Error.hpp>

#include <nlohmann/json.hpp>

namespace re {

/**
 * @brief Provides a mechanism for user-changeable key bindings
 *
 * Keybinder holds a list of key-binding points. Upon creation, the
 * key-binding points are assigned previously saved values (= bound keys).
 * If the saved bindings cannot be loaded, default bindings are assigned.
 *
 * The key bound to a point can accessed via operator[]. The bindings can be
 * overbound synchronously via changeBinding() or asynchronously via
 * listenChangeBinding().
 *
 * @tparam KeyBinding       Enum class that contains the binding points. The
 *                          values must be continous sequence 0..N-1
 * @tparam KeyBindingInfo   A type that holds additional info about the
 *                          binding point. It must have member 'defaultValue'
 *                          that return Key and member 'name' that return a string
 *                          type.
 */
template<typename KeyBinding, typename KeyBindingInfo>
class KeyBinder {
    friend class MainProgram;

public:

    using KeyBindingInfos =
        std::span<const KeyBindingInfo, std::to_underlying(KeyBinding::Count)>;

    using KeyBindingIntType = std::underlying_type_t<KeyBinding>;

    /**
     * @brief Assignes bindings the previously saved values or the default ones.
     * @param bindingInfos Must be valid throughout the lifetime of the KeyBinder
     */
    KeyBinder(KeyBindingInfos bindingInfos)
        : m_bindingInfos{bindingInfos} {
        try {
            if (!loadBindings()) {
                // Bindings are not latest-version, save the current ones
                saveCurrentBindings();
                std::cout << "Bindings successfully updated.\n";
            } else {
                std::cout << "Loaded up-to-date bindings.\n";
            }
        } catch (...) { // No bindings exist, resetting them
            std::cout << "No bindings exist, creating default ones.\n";
            resetBindings(true);
        }
    }

    /**
     * @brief Gets the currently bound key for the given key binding
     * @param binding The binding to get the bound key of
     * @return Currently bound key of the binding
     */
    Key operator[](KeyBinding binding) const {
        return m_bindings[std::to_underlying(binding)];
    };

    /**
     * @brief Immediately changes the binding to the given key and saves the change
     * @param binding The binding to change
     * @param key The key to assign
     */
    void changeBinding(KeyBinding binding, Key key) {
        m_bindings[std::to_underlying(binding)] = key;
        saveCurrentBindings();
    };

    /**
     * @brief Immediately resets the binding to its default key and saves
     * the change
     * @param binding The binding to change
     */
    void resetBinding(KeyBinding binding) {
        changeBinding(binding, m_bindingInfos[std::to_underlying(binding)].defaultValue);
    };

    /**
     * @brief Loads bindings from previously saved file
     * @details If a binding cannot be loaded, its default value is assigned instead.
     * @return True if all bindings could be loaded, false otherwise
     */
    bool loadBindings() {
        nlohmann::json j;
        std::ifstream i(m_bindingFileName);
        i >> j;

        for (auto item = j.begin(); item != j.end(); item++) {
            auto binding = searchBindingEnum(item.key());
            if (binding.has_value()) {
                m_bindings[std::to_underlying(*binding)] =
                    toKey(item.value().get<std::string>());
            }
        }

        bool everythingLoaded = true;
        for (size_t i = 0; i < m_bindingInfos.size(); i++) {
            if (m_bindings[i] == Key::NoKey) {
                m_bindings[i]    = m_bindingInfos[i].defaultValue;
                everythingLoaded = false;
            }
        }
        return everythingLoaded;
    }

    /**
     * @brief Resets all bindings to their default values
     * @param permanently If true, the reset bindings are saved
     */
    void resetBindings(bool permanently) {
        for (size_t i = 0; i < std::to_underlying(KeyBinding::Count); i++) {
            m_bindings[i] = m_bindingInfos[i].defaultValue;
        }
        if (permanently) {
            saveCurrentBindings();
        }
    }

    /**
     * @brief Saves current bindings
     */
    void saveCurrentBindings() {
        nlohmann::ordered_json j;

        for (size_t i = 0; i < std::to_underlying(KeyBinding::Count); i++) {
            j[m_bindingInfos[i].name] = toString(m_bindings[i]);
        }

        std::ofstream o(m_bindingFileName, std::ofstream::trunc);
        o << j.dump(2);
        o.close();
    }

    /**
     * @brief Asynchronously changes a keybinding
     * @param binding The binding to change
     * @param callbackReceiver An object that will be notified that the keybind
     *                         has been changed
     * @param stopKey The key that stops the listening without changing the
     * keybind
     */
    template<typename CallbackReceiver, void (CallbackReceiver::*callback)(Key)>
    void listenChangeBinding(
        KeyBinding binding, CallbackReceiver& callbackReceiver,
        Key stopKey = Key::Delete
    ) {
        auto info = new ListeningInfo<CallbackReceiver>{
            .binding          = binding,
            .keyBinder        = (*this),
            .stopKey          = stopKey,
            .callbackReceiver = callbackReceiver
        };

        auto thread = SDL_CreateThread(
            (listenForKey<CallbackReceiver, callback>), "keybind_listener", info
        );
        SDL_DetachThread(thread);
    };

private:
    std::optional<KeyBinding> searchBindingEnum(std::string_view name) {
        for (size_t i = 0; i < m_bindingInfos.size(); i++) {
            if (m_bindingInfos[i].name == name) {
                return static_cast<KeyBinding>(i); // Found the enum
            }
        }
        return {}; // Did not find the enum
    }

    template<typename CallbackReceiver>
    struct ListeningInfo {
        KeyBinding binding;
        KeyBinder& keyBinder;
        Key stopKey;
        CallbackReceiver& callbackReceiver;
    };

    template<typename CallbackReceiver, void (CallbackReceiver::*func)(Key)>
    static int listenForKey(void* ptr) {
        SDL_Event evnt;
        Key newKey = Key::UnknownKey;
        MainProgram::pollEventsInMainThread(false);
        ListeningInfo<CallbackReceiver>* info =
            reinterpret_cast<ListeningInfo<CallbackReceiver>*>(ptr);
        int rval = 0;

        while (newKey == Key::UnknownKey) {           // Until a key is pressed
            while (SDL_WaitEventTimeout(&evnt, 10)) { // Wait for events
                switch (evnt.type) { // Extract the pressed key (if it is one)
                case SDL_EVENT_KEY_DOWN: newKey = toKey(evnt.key.key); break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    newKey = toKey(evnt.button.button);
                    break;
                case SDL_EVENT_MOUSE_WHEEL:
                    if (evnt.wheel.y != 0) {
                        newKey = (evnt.wheel.y > 0) ? Key::UMW : Key::DMW;
                    } else {
                        newKey = (evnt.wheel.x > 0) ? Key::RMW : Key::LMW;
                    }
                    break;
                }
            }
        }

        // If the pressed key is the one that stops the listening
        if (newKey == info->stopKey) {
            rval = 1;
        } else { // Successfully changed the binding
            info->keyBinder.changeBinding(info->binding, newKey);
        }

        // Callback
        (info->callbackReceiver.*func)(newKey);

        MainProgram::pollEventsInMainThread(true);
        delete info;
        return rval;
    }

    std::string m_bindingFileName = "bindings.json";

    std::array<Key, std::to_underlying(KeyBinding::Count)> m_bindings;
    KeyBindingInfos m_bindingInfos;
};

} // namespace re
