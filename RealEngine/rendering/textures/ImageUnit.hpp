/*!
 *  @author    Dubsky Tomas
 */
#pragma once

namespace RE {

/**
 * @brief Specifies how a bound image will be accessed
*/
enum class ImageAccess : unsigned int {
    READ_ONLY,
    WRITE_ONLY,
    READ_WRITE
};

template<typename> class Texture;
template<typename> class TextureProxy;

/**
 * @brief Represents a binding point that an image from a texture can be bound to
*/
class ImageUnit {
    template<typename R>friend class Texture;
    template<typename R>friend class TextureProxy;
    friend class GL46_ShaderProgram;
    friend class GL46_Texture;
public:

    ImageUnit(unsigned int unit) : m_unit(unit) {}

private:

    unsigned int m_unit;    /**< Name of the binding point */
};

}