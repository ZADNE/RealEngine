#pragma once
#include <string>
#include <compare>

#include <GL/glew.h>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <RealEngine/graphics/Vertex.hpp>
#include <RealEngine/graphics/texture/TextureParameters.hpp>
#include <RealEngine/graphics/texture/TextureImage.hpp>


namespace RE {

/**
 * @brief Texture proxy is a small class that holds only
 * non-owning handle to a texture.
 *
 * Once the texture that it represents is destroyed, the proxy
 * becomed invalid and should not be used
*/
class TextureProxy {
	friend class Texture;
public:

	/**
	 * @brief TO BE REMOVED
	*/
	explicit TextureProxy(GLuint ID) : m_ID(ID) {}

	/**
	 * @brief Contructs proxy that represents the given texture
	 * @param texture Texture to present
	*/
	TextureProxy(const Texture& texture);

	/**
	 * @brief Binds the texture that this proxy represents.
	 *
	 * Behaviour is undefined if the originating texture was destroyed.
	*/
	void bind() const { glBindTexture(GL_TEXTURE_2D, m_ID); }

	auto operator<=>(const TextureProxy&) const = default;
private:
	GLuint m_ID; /**< OpenGL name of the texture */
};

/**
 * @brief Texture is image with associated parameters.
 * @see TextureImage
 * @see TextureParameters
*/
class Texture {
	friend class TextureProxy;
public:
	static inline const TextureFlags DEFAULT_FLAGS{TextureFlags::RGBA_NU_NEAR_NEAR_EDGE};
	static inline const RE::Colour DEFAULT_BORDER_COLOUR{255, 20, 147, 255};

	/**
	 * @brief Default parameters for texture use RGBA_NU_NEAR_NEAR_EDGE flags and geometry defined by image
	 * @see TextureFlags::RGBA_NU_NEAR_NEAR_EDGE
	*/
	static inline const TextureParameters DEFAULT_PARAMETERS{{}, DEFAULT_FLAGS, DEFAULT_BORDER_COLOUR};


	/**
	 * @brief Constructs texture from PNG
	 *
	 * Parameters are either loaded from reAl chunk of PNG,
	 * or if that cannot be done (reAl chunk is missing or it has bad format),
	 * default parameters are used.
	 *
	 * @see DEFAULT_PARAMETERS
	 *
	 * @param filePathPNG Path to the PNG
	*/
	Texture(const std::string& filePathPNG);

	/**
	 * @brief Contructs texture from image and parameters
	 *
	 * Default parameters are used if not specified.
	 * @see DEFAULT_PARAMETERS
	 *
	 * @param image Specifies true dimensions of the texture and initial contents of the texture if pixels are provided
	 * @param params Specifies how the texture will be drawn.
	*/
	Texture(const TextureImage& image, const TextureParameters& params = DEFAULT_PARAMETERS);

	Texture(const Texture&) = delete;

	/**
	 * @brief Move contructs from another texture
	 * @param other Texture to be moved from
	*/
	Texture(Texture&& other) noexcept;

	Texture& operator=(const Texture&) = delete;

	/**
	 * @brief Move assigns value of another texture
	 * @param other Texture to be moved from
	 * @return this
	*/
	Texture& operator=(Texture&& other) noexcept;

	/**
	 * @brief Destroys texture
	*/
	~Texture();

	TextureChannels getChannels() const { return m_flags.getChannels(); }
	TextureFormatType getFormatType() const { return m_flags.getFormatType(); }
	TextureFormatSign getFormatSign() const { return m_flags.getFormatSign(); }
	TextureFormat getFormat() const { return m_flags.getFormat(); }
	TextureMinFilterMipmapsUsage getMinFilterMipmapsUsage() const { return m_flags.getMinFilterMipmapsUsage(); }
	TextureMinFilterInMipmap getMinFilterInMipmap() const { return m_flags.getMinFilterInMipmap(); }
	TextureMinFilterBetweenMipmaps getMinFilterBetweenMipmaps() const { return m_flags.getMinFilterBetweenMipmaps(); }
	TextureMinFilter getMinFilter() const { return m_flags.getMinFilter(); }
	TextureMagFilter getMagFilter() const { return m_flags.getMagFilter(); }
	TextureWrapStyle getWrapStyleX() const { return m_flags.getWrapStyleX(); }
	TextureWrapStyle getWrapStyleY() const { return m_flags.getWrapStyleY(); }

	TextureParameters getParameters() const;
	glm::vec2 getSubimageDims() const { return m_subimageDims; }
	glm::vec2 getPivot() const { return m_pivot; }
	glm::vec2 getSubimagesSpritesCount() const { return m_subimagesSpritesCount; }
	RE::Colour getBorderColour() const { return m_borderColour; };

	glm::uvec2 getTrueDims() const { return m_trueDims; }

	/**
	 * @note Switching to mipmap using filter is not possible if the texture was constructed without mipmaps.
	*/
	void setMinFilter(TextureMinFilter minFilter);
	void setMagFilter(TextureMagFilter magFilter);
	void setWrapStyleX(TextureWrapStyle wrapStyleX);
	void setWrapStyleY(TextureWrapStyle wrapStyleY);


	void setSubimageDims(const glm::vec2& subimageDims) { m_subimageDims = subimageDims; }
	void setPivot(const glm::vec2& pivot) { m_pivot = pivot; }
	void setSubimagesSpritesCount(const glm::vec2& subimagesSpritesCount) { m_subimagesSpritesCount = subimagesSpritesCount; }
	void setBorderColour(RE::Colour col);

	/**
	 * @brief Gets internal identifier of the texture.
	 *
	 * This breaches class encapsulation - use with caution!
	 *
	 * @return Internal ID of the texture.
	*/
	GLuint getID() const {
		return m_ID;
	}

	/**
	 * @brief Binds the texture to current texture unit.
	*/
	void bind() const { glBindTexture(GL_TEXTURE_2D, m_ID); }

	/**
	 * @brief Clears FIRST MIPMAP of the texture with given colour.
	 * @param colour Colour to clear the texture with.
	*/
	void clear(const glm::vec4& colour) const;

	/**
	 * @brief Clears FIRST MIPMAP of the texture with given colour.
	 * @param colour Colour to clear the texture with.
	*/
	void clear(RE::Colour colour) const;

	/**
	 * @brief Saves texture as a PNG file.
	 * @param filePathPNG Filepath to save, existing file will be overwritten withot warnings.
	 * @return True if saved succesfully, false otherwise.
	*/
	bool saveToFile(const std::string& filePathPNG);

	/**
	 * @brief Saves texture as a PNG file with parameters different than current ones.
	 * @param filePathPNG Filepath to save, existing file will be overwritten withot warnings.
	 * @param TextureParameters Parameters to save (this does not override in-memory parameters of the texture)
	 * @return True if saved succesfully, false otherwise.
	*/
	bool saveToFile(const std::string& filePathPNG, const TextureParameters& params);

	/**
	 * @brief Conctructs human readable identification string of the texture.
	 * @return Human readable identification string of the texture
	*/
	std::string identificationString() const;
private:
	void init(const TextureImage& image, const TextureParameters& params);

	GLuint m_ID = 0u;			/**< OpenGL name of the texture */
	TextureFlags m_flags{};		/**< Flags associated with the texture */

	glm::vec2 m_subimageDims{};	/**< Dimensions of the texture */
	glm::vec2 m_pivot{};		/**< Pivot of the texture */
	glm::vec2 m_subimagesSpritesCount{};/**< Number of subimages and sprites of the texture */

	glm::uvec2 m_trueDims{};	/**< True dimensions of the OpenGL texture */

	RE::Colour m_borderColour{};/**< Border colour of the texture */
};

}
