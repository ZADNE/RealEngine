/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <SDL3/SDL_Properties.h>

namespace re {

/**
 * @brief Is RAII wrapper around SDL_PropertiesID with convenience functions
 */
class SDLProperties {
public:
    explicit SDLProperties() {}

    SDLProperties(const SDLProperties&)            = delete; ///< Noncopyable
    SDLProperties& operator=(const SDLProperties&) = delete; ///< Noncopyable

    SDLProperties(SDLProperties&&)            = delete;      ///< Nonmovable
    SDLProperties& operator=(SDLProperties&&) = delete;      ///< Nonmovable

    ~SDLProperties() { SDL_DestroyProperties(m_handle); }

    template<typename T>
    SDLProperties& setProperty(const char* name, T prop) {
        if constexpr (std::is_same_v<T, const char*>) {
            SDL_SetStringProperty(m_handle, name, prop);
        } else if constexpr (std::is_pointer_v<T>) {
            SDL_SetPointerProperty(m_handle, name, prop);
        } else if constexpr (std::is_same_v<T, bool>) {
            SDL_SetBooleanProperty(m_handle, name, prop);
        } else if constexpr (std::is_integral_v<T>) {
            SDL_SetNumberProperty(m_handle, name, prop);
        } else if constexpr (std::is_same_v<T, float>) {
            SDL_SetFloatProperty(m_handle, name, prop);
        } else {
            static_assert(false, "Unexpected type of property");
        }
        return *this;
    }

    operator SDL_PropertiesID() const { return m_handle; }
private:
    SDL_PropertiesID m_handle = SDL_CreateProperties();
};

} // namespace re
