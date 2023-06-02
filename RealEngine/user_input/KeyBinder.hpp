/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <array>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_thread.h>

#include <RealEngine/program/MainProgram.hpp>
#include <RealEngine/user_input/Key.hpp>
#include <RealEngine/utility/Error.hpp>

#include <nlohmann/json.hpp>

namespace re {

    template<typename KeyBindings, typename KeyBindingInfo>
    using KeyBindingInfoList =
        std::array<KeyBindingInfo, static_cast<size_t>(KeyBindings::Count)>;

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
     * @tparam KeyBindings      Enum class that contains the binding points. The
     * values must be continous sequence 0..N-1
     * @tparam KeyBindingInfo   A type that holds additional info about the
     * binding point. It must have member 'defaultValue' that return Key and
     * member 'name' that return a string type.
     * @tparam infoList         Array with info about each binding point.
     */
    template<
        typename KeyBindings,
        typename KeyBindingInfo,
        const KeyBindingInfoList<KeyBindings, KeyBindingInfo>& infoList>
    class KeyBinder {
        friend class MainProgram;

    public:
        /**
         * @brief Assignes bindings the previously saved values or the default ones.
         */
        KeyBinder() {
            try {
                if (loadBindings()) {
                    saveCurrentBindings(); // Bindings are not latest-version,
                                           // saving current one
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
        Key operator[](KeyBindings binding) const {
            return m_bindings[static_cast<size_t>(binding)];
        };

        /**
         * @brief Immediately changes the binding to the given key and saves the
         * change
         * @param binding The binding to change
         * @param key The key to assign
         */
        void changeBinding(KeyBindings binding, Key key) {
            m_bindings[static_cast<size_t>(binding)] = key;
            saveCurrentBindings();
        };

        /**
         * @brief Immediately resets the binding to its default key and saves
         * the change
         * @param binding The binding to change
         */
        void resetBinding(KeyBindings binding) {
            changeBinding(binding, infoList[static_cast<size_t>(binding)].defaultValue);
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
            std::ifstream  i(m_bindingFileName);
            i >> j;

            for (auto item = j.begin(); item != j.end(); item++) {
                auto binding = searchBindingEnum(item.key());
                if (binding.has_value()) {
                    m_bindings[static_cast<size_t>(*binding)] =
                        stringToKey(item.value().get<std::string>());
                }
            }

            bool returnVal = false;
            for (size_t i = 0; i < infoList.size(); i++) {
                if (m_bindings[i] == Key::NoKey) {
                    m_bindings[i] = infoList[i].defaultValue;
                    returnVal     = true;
                }
            }
            return returnVal; // Indicates whether bindings have been up-to-date
        }

        /**
         * @brief Resets all bindings to their default values
         * @param permanently If true, the reset bindings are saved
         */
        void resetBindings(bool permanently) {
            for (size_t i = 0; i < static_cast<size_t>(KeyBindings::Count); i++) {
                m_bindings[i] = infoList[i].defaultValue;
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

            for (size_t i = 0; i < static_cast<size_t>(KeyBindings::Count); i++) {
                j[infoList[i].name] = keyToString(m_bindings[i]);
            }

            std::ofstream o(m_bindingFileName, std::ofstream::trunc);
            o << j.dump(2);
            o.close();
        }

        /**
         * @brief Asynchronously changes a keybinding
         * @param binding The binding to change
         * @param stopKey The key that stops the listening without changing the
         * keybind
         */
        template<typename CallbackReceiver, void (CallbackReceiver::*callback)(Key)>
        void listenChangeBinding(
            KeyBindings       binding,
            CallbackReceiver& callbackReceiver,
            Key               stopKey = Key::Delete
        ) {
            auto info = new ListeningInfo<CallbackReceiver>{
                .binding          = binding,
                .keyBinder        = (*this),
                .stopKey          = stopKey,
                .callbackReceiver = callbackReceiver};

            auto thread = SDL_CreateThread(
                (listenForKey<CallbackReceiver, callback>), "keybind_listener", info
            );
            SDL_DetachThread(thread);
        };

    private:
        std::optional<KeyBindings> searchBindingEnum(std::string_view name) {
            for (size_t i = 0; i < infoList.size(); i++) {
                if (infoList[i].name == name) {
                    return static_cast<KeyBindings>(i); // Found the enum
                }
            }
            return {}; // Did not find the enum
        }

        template<typename CallbackReceiver>
        struct ListeningInfo {
            KeyBindings       binding;
            KeyBinder&        keyBinder;
            Key               stopKey;
            CallbackReceiver& callbackReceiver;
        };

        template<typename CallbackReceiver, void (CallbackReceiver::*func)(Key)>
        static int listenForKey(void* ptr) {
            SDL_Event evnt;
            Key       newKey = Key::UnknownKey;
            MainProgram::pollEventsInMainThread(false);
            ListeningInfo<CallbackReceiver>* info =
                reinterpret_cast<ListeningInfo<CallbackReceiver>*>(ptr);
            int rval = 0;

            while (newKey == Key::UnknownKey) { // Until a key is pressed
                while (SDL_WaitEventTimeout(&evnt, 10)) { // Wait for events
                    switch (evnt.type) { // Extract the pressed key (if it is one)
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

        std::array<Key, static_cast<size_t>(KeyBindings::Count)> m_bindings;
    };

} // namespace re