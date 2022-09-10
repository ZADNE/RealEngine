/*! 
 *  @author    Dubsky Tomas
 */
export module RealEngine.rendering.textures.TextureParameters;
import std;
import glm;
export import RealEngine.rendering.textures.TextureGeometry;
export import RealEngine.rendering.textures.TextureFlags;
export import RealEngine.rendering.vertices.Vertex;


export namespace RE {

/**
* @brief Specifies all auxiliary information (except the rasters)
* that is required to draw a texture.
*/
class TextureParameters : public TextureGeometry, public TextureFlags {
public:

    /**
    * @brief Constructs parameters from raw bytes encoded as Real Texture Infromation (RTI)
    *
    * @throws When bytes cannot be decoded.
    *
    * @param rtiBytes Raw bytes of RTI
    * @param size Size of the buffer rtiBytes
    */
    TextureParameters(const unsigned char* rtiBytes, size_t size);

    /**
    * @brief Constructs parameters from given geometry, flags and border color
    * @param geometry Geometry of the parameters
    * @param flags Flags of the parameters
    * @param borderColor Border color of the parameters
    */
    constexpr TextureParameters(const TextureGeometry& geometry, TextureFlags flags, Color borderColor) :
        TextureGeometry(geometry), TextureFlags(flags), m_borderColor(borderColor) {}

    /**
    * @brief Constructs parameters from given geometry and flags.
    *
    * Border color is set to (0, 0, 0, 255).
    *
    * @param geometry Geometry of the parameters
    * @param flags Flags of the parameters
    */
    constexpr TextureParameters(const TextureGeometry& geometry, TextureFlags flags) :
        TextureParameters(geometry, flags, { 0u, 0u, 0u, 255u }) {}

    /**
    * @brief Constructs parameters with flags only.
    *
    * Geometry is set to be defined by raster.
    * Color is set to (0, 0, 0, 255).
    *
    * @param flags Flags of the parameters
    */
    TextureParameters(TextureFlags flags) :
        TextureParameters(TextureGeometry{}, flags) {}

    /**
    * @brief Gets the border color.
    *
    * Border color is the color that will be sampled outside texture's texels
    * if clamping to border is used. It has no effect if clamping to border
    * is not used.
    * @return Border color of the texture
    */
    Color getBorderColor() const { return m_borderColor; }

    /**
    * @brief Converts parameters to raw bytes, encoded as Real Texture Infromation (RTI)
    * @return Raw bytes of RTI that represent these parameters
    */
    std::vector<unsigned char> convertToRTI() const;

private:

    Color m_borderColor;/**< Border color of the texture*/
};

}