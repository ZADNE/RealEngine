/*!
 *  @author    Dubsky Tomas
 */
export module RealEngine.rendering.textures.ImageUnit;
import RealEngine.rendering.Renderer;


export namespace RE {

/**
 * @brief Specifies how a bound image will be accessed
*/
enum class ImageAccess : unsigned int {
    READ_ONLY,
    WRITE_ONLY,
    READ_WRITE
};

template<Renderer> class Texture;
template<Renderer> class TextureProxy;

/**
 * @brief Represents a binding point that an image from a texture can be bound to
*/
class ImageUnit {
    template<Renderer> friend class Texture;
    template<Renderer> friend class TextureProxy;
    friend class GL46ShaderProgram;
    friend class GL46Texture;
public:

    constexpr ImageUnit(unsigned int unit) : m_unit(unit) {}

private:

    unsigned int m_unit;    /**< Name of the binding point */
};

}