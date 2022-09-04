/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/Renderer.hpp>

namespace RE {

template<Renderer> class Texture;
template<Renderer> class TextureProxy;

/**
 * @brief Represents a binding point that a texture can be bound to
*/
class TextureUnit {
    template<Renderer> friend class Texture;
    template<Renderer> friend class TextureProxy;
    friend class GL46_ShaderProgram;
    friend class GL46_Texture;
public:

    constexpr TextureUnit(unsigned int unit) : m_unit(unit) {}

private:

    unsigned int m_unit;    /**< Name of the binding point */
};

}