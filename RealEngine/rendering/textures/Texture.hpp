/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>
#include <compare>

#include <RealEngine/rendering/internal_interfaces/ITexture.hpp>


namespace RE {

class Texture;

/**
 * @brief Holds a non-owning handle to a Texture.
 *
 * @warning Once the texture that it represents is destroyed, the proxy
 * becomes invalid and should not be used!
*/
class TextureProxy {
	friend class Texture;
	friend class GL46_Renderer;
	friend class GL46_Texture;
public:

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
	void bind() const;

	/**
	 * @brief Binds the texture that this proxy represents to the given texture unit.
	 *
	 * Behaviour is undefined if the originating texture was destroyed.
	 *
	 * @param unit Texture unit to bind the texture to.
	*/
	void bind(TextureUnit unit) const;

	auto operator<=>(const TextureProxy&) const = default;
private:
	unsigned int m_ID; /**< OpenGL name of the texture */

	static ITexture* s_impl;
};

/**
 * @brief Represents one (or more) images.
 *
 * Textures are always stored in GPU memory.
 *
 * @see Raster
 * @see TextureParameters
*/
class Texture {
	friend class TextureProxy;
	friend class GL46_Renderer;
	friend class GL46_Texture;
	friend class GL46_Framebuffer;
public:
	static inline constexpr TextureFlags DEFAULT_FLAGS{TextureFlags::RGBA8_NU_NEAR_NEAR_EDGE};
	static inline constexpr Color DEFAULT_BORDER_COLOR{255, 20, 147, 255};

	/**
	 * @brief Default parameters for texture use RGBA_NU_NEAR_NEAR_EDGE flags and geometry defined by raster
	 * @see TextureFlags::RGBA_NU_NEAR_NEAR_EDGE
	*/
	static inline constexpr TextureParameters DEFAULT_PARAMETERS{{}, DEFAULT_FLAGS, DEFAULT_BORDER_COLOR};

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
	 * @brief Contructs texture from raster and parameters
	 *
	 * Default parameters are used if not specified.
	 * @see DEFAULT_PARAMETERS
	 *
	 * @param raster Specifies true dimensions of the texture and initial contents of the texture if pixels are provided
	 * @param params Specifies how the texture will be drawn.
	*/
	Texture(const Raster& raster, const TextureParameters& params = DEFAULT_PARAMETERS);

	Texture(const Texture&) = delete;
	Texture(Texture&& other) noexcept;

	Texture& operator=(const Texture&) = delete;
	Texture& operator=(Texture&& other) noexcept;

	/**
	 * @brief Destroys the texture
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
	TextureBitdepthPerChannel getBitdepthPerChannel() const { return m_flags.getBitdepthPerChannel(); }

	TextureParameters getParameters() const;
	glm::vec2 getSubimageDims() const { return m_subimageDims; }
	glm::vec2 getPivot() const { return m_pivot; }
	glm::vec2 getSubimagesSpritesCount() const { return m_subimagesSpritesCount; }
	Color getBorderColor() const { return m_borderColor; };

	glm::uvec2 getTrueDims() const { return m_trueDims; }

	/**
	 * @note Switching to a filter that uses mipmaps is not possible if the texture was constructed without mipmaps.
	*/
	void setMinFilter(TextureMinFilter minFilter);
	void setMagFilter(TextureMagFilter magFilter);
	void setWrapStyleX(TextureWrapStyle wrapStyleX);
	void setWrapStyleY(TextureWrapStyle wrapStyleY);


	void setSubimageDims(const glm::vec2& subimageDims) { m_subimageDims = subimageDims; }
	void setPivot(const glm::vec2& pivot) { m_pivot = pivot; }
	void setSubimagesSpritesCount(const glm::vec2& subimagesSpritesCount) { m_subimagesSpritesCount = subimagesSpritesCount; }
	void setBorderColor(Color col);
	void setBorderColor(const glm::vec4& col);

	/**
	 * @brief Binds the texture to the active texture unit.
	*/
	void bind() const;

	/**
	 * @brief Binds the texture to the given texture unit.
	 *
	 * @param unit Texture unit to bind the texture to.
	*/
	void bind(TextureUnit unit) const;

	/**
	 * @brief Binds an image of the texture to the image unit
	 * @param unit Unit to bind the image to
	 * @param level Mipmap level of the texture to bind
	 * @param access Access to the image
	*/
	void bindImage(ImageUnit unit, int level, ImageAccess access) const;

	/**
	 * @brief Replaces a portion of an image within the texture with given raster
	 * @param level Level of the texture to replace
	 * @param offset Offset within the target image to place the raster to
	 * @param size Size of the rectangle that is to be replaced
	 * @param raster The texels that will be copied into the image. The format must be RGBA 8-bits per channel
	*/
	void setTexels(int level, const glm::ivec2& offset, const glm::ivec2& size, const void* raster) const;

	/**
	 * @brief Copies texels between two images
	 * @param srcLevel Level (image) of this texture to be used as source
	 * @param srcPos Position within the source level (image) to copy from
	 * @param destination Destination texture that will receive the texels
	 * @param dstLevel Level (image) of the destination texture that will receive the texels
	 * @param dstPos Position within the destination the destination level (image) to copy to
	 * @param size Size of the area that is to be copied
	*/
	void copyTexels(int srcLevel, const glm::ivec2& srcPos, const Texture& destination, int dstLevel, const glm::ivec2& dstPos, const glm::ivec2& size) const;

	/**
	 * @brief Clears first level of the texture with given color.
	 * @param color Color to clear the texture with.
	*/
	void clear(const glm::vec4& color) const;
	void clear(const glm::ivec4& color) const;
	void clear(const glm::uvec4& color) const;
	void clear(Color color) const;

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

private:

	void init(const Raster& raster, const TextureParameters& params);

	unsigned int m_ID = 0u;			/**< OpenGL name of the texture */
	TextureFlags m_flags{};		/**< Flags associated with the texture */

	glm::vec2 m_subimageDims{};	/**< Dimensions of the texture */
	glm::vec2 m_pivot{};		/**< Pivot of the texture */
	glm::vec2 m_subimagesSpritesCount{};/**< Number of subimages and sprites of the texture */

	glm::uvec2 m_trueDims{};	/**< True dimensions of the OpenGL texture */

	Color m_borderColor{};/**< Border color of the texture */

	static ITexture* s_impl;
};

}
