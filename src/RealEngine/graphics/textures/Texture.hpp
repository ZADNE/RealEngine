#pragma once
#include <string>
#include <compare>

#include <GL/glew.h>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <RealEngine/graphics/Vertex.hpp>
#include <RealEngine/graphics/textures/TextureParameters.hpp>
#include <RealEngine/graphics/textures/Raster.hpp>
#include <RealEngine/graphics/textures/TextureUnit.hpp>
#include <RealEngine/graphics/textures/ImageUnit.hpp>


namespace RE {

	/**
	 * @brief Is a small class that holds only
	 * non-owning handle to a texture.
	 *
	 * Once the texture that it represents is destroyed, the proxy
	 * becomed invalid and should not be used
	*/

	class Texture;

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

		/**
		 * @brief Binds the texture that this proxy represents to the given texture unit.
		 *
		 * Behaviour is undefined if the originating texture was destroyed.
		 *
		 * @param unit Texture unit to bind the texture to.
		*/
		void bind(TextureUnit unit) const { glBindTextureUnit(unit.m_unit, m_ID); }

		auto operator<=>(const TextureProxy&) const = default;
	private:
		GLuint m_ID; /**< OpenGL name of the texture */
	};

	/**
	 * @brief Represents one (or more) images with associated parameters.
	 * @see Raster
	 * @see TextureParameters
	*/
	class Texture {
		friend class TextureProxy;
	public:
		static inline const TextureFlags DEFAULT_FLAGS{ TextureFlags::RGBA8_NU_NEAR_NEAR_EDGE };
		static inline const RE::Color DEFAULT_BORDER_COLOR{ 255, 20, 147, 255 };

		/**
		 * @brief Default parameters for texture use RGBA_NU_NEAR_NEAR_EDGE flags and geometry defined by raster
		 * @see TextureFlags::RGBA_NU_NEAR_NEAR_EDGE
		*/
		static inline const TextureParameters DEFAULT_PARAMETERS{ {}, DEFAULT_FLAGS, DEFAULT_BORDER_COLOR };


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
		TextureBitdepthPerChannel getBitdepthPerChannel() const { return m_flags.getBitdepthPerChannel(); }

		TextureParameters getParameters() const;
		glm::vec2 getSubimageDims() const { return m_subimageDims; }
		glm::vec2 getPivot() const { return m_pivot; }
		glm::vec2 getSubimagesSpritesCount() const { return m_subimagesSpritesCount; }
		RE::Color getBorderColor() const { return m_borderColor; };

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
		void setBorderColor(RE::Color col);
		void setBorderColor(const glm::vec4& col);

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
		 * @brief Binds the texture to the active texture unit.
		*/
		void bind() const { glBindTexture(GL_TEXTURE_2D, m_ID); }

		/**
		 * @brief Binds the texture to the given texture unit.
		 *
		 * @param unit Texture unit to bind the texture to.
		*/
		void bind(TextureUnit unit) const { glBindTextureUnit(unit.m_unit, m_ID); }

		/**
		 * @brief Binds an image of the texture to the image unit
		 * @param unit Unit to bind the image to
		 * @param level Mipmap level of the texture to bind
		 * @param access Access to the image
		*/
		void bindImage(ImageUnit unit, GLint level, ImageAccess access) const;

		/**
		 * @brief Replaces a portion of an image within the texture with given raster
		 * @param level Level of the texture to replace
		 * @param offset Offset within the target image to place the raster to
		 * @param size Size of the rectangle that is to be replaced
		 * @param raster The texels that will be copied int othe image. The format must be RGBA 8-bits per channel
		*/
		void setTexelsWithinImage(GLint level, const glm::ivec2& offset, const glm::ivec2& size, const void* raster) const;

		/**
		 * @brief Copies texels between two images
		 * @param srcLevel Level (image) of this texture to be used as source
		 * @param srcPos Position within the source level (image) to copy from
		 * @param destination Destination texture that will receive the texels
		 * @param dstLevel Level (image) of the destination texture that will receive the texels
		 * @param dstPos Position within the destination the destination level (image) to copy to
		 * @param size Size of the area that is to be copied
		*/
		void copyTexelsBetweenImages(GLint srcLevel, const glm::ivec2& srcPos, const Texture& destination, GLint dstLevel, const glm::ivec2& dstPos, const glm::ivec2& size) const;

		/**
		 * @brief Clears first level of the texture with given color.
		 * @param color Color to clear the texture with.
		*/
		void clear(const glm::vec4& color) const;
		void clear(const glm::ivec4& color) const;
		void clear(const glm::uvec4& color) const;
		void clear(RE::Color color) const;

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
		void init(const Raster& raster, const TextureParameters& params);

		GLuint m_ID = 0u;			/**< OpenGL name of the texture */
		TextureFlags m_flags{};		/**< Flags associated with the texture */

		glm::vec2 m_subimageDims{};	/**< Dimensions of the texture */
		glm::vec2 m_pivot{};		/**< Pivot of the texture */
		glm::vec2 m_subimagesSpritesCount{};/**< Number of subimages and sprites of the texture */

		glm::uvec2 m_trueDims{};	/**< True dimensions of the OpenGL texture */

		RE::Color m_borderColor{};/**< Border color of the texture */
	};

}
