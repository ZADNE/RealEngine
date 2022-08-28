/*!
 *  @author    Dubsky Tomas
 */
#pragma once

namespace RE {

/**
 * @brief Represents a binding point that a texture can be bound to
*/
class TextureUnit {
    friend class Texture;
    friend class TextureProxy;
    friend class GL46_ShaderProgram;
    friend class GL46_Texture;
public:

    TextureUnit(unsigned int unit) : m_unit(unit) {}

private:
    unsigned int m_unit;    /**< Name of the binding point */
};

}