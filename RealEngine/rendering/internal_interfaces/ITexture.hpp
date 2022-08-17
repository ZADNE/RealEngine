/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/textures/Raster.hpp>
#include <RealEngine/rendering/textures/TextureUnit.hpp>
#include <RealEngine/rendering/textures/ImageUnit.hpp>


namespace RE {

class Texture;
class TextureProxy;

/**
* @brief Is a renderer-agnostic interface to Texture's implementation.
*
* This is used internally by Texture.
*
* @see RE::Texture
*/
class ITexture {
public:
	
	virtual void construct(Texture& te, const Raster& raster) const = 0;
	virtual void destruct(Texture& te) const = 0;

	virtual void setMinFilter(Texture& te, TextureMinFilter minFilter) const = 0;
	virtual void setMagFilter(Texture& te, TextureMagFilter magFilter) const = 0;
	virtual void setWrapStyleX(Texture& te, TextureWrapStyle wrapStyleX) const = 0;
	virtual void setWrapStyleY(Texture& te, TextureWrapStyle wrapStyleY) const = 0;

	virtual	void setBorderColor(Texture& te, const glm::vec4& col) const = 0;

	virtual void bind(const Texture& te) const = 0;
	virtual void bind(const TextureProxy& te) const = 0;
	virtual void bind(const Texture& te, TextureUnit unit) const = 0;
	virtual void bind(const TextureProxy& te, TextureUnit unit) const = 0;
	virtual void bindImage(const Texture& te, ImageUnit unit, GLint level, ImageAccess access) const = 0;

	virtual void setTexels(const Texture& te, GLint level, const glm::ivec2& offset, const glm::ivec2& size, const void* raster) const = 0;
	virtual void copyTexels(const Texture& te, GLint srcLevel, const glm::ivec2& srcPos, const Texture& destination, GLint dstLevel, const glm::ivec2& dstPos, const glm::ivec2& size) const = 0;
	virtual void getTexels(const Texture& te, GLint level, GLsizei bufSize, void* pixels) const = 0;

	virtual void clear(const Texture& te, GLint level, const glm::vec4& color) const = 0;
	virtual void clear(const Texture& te, GLint level, const glm::ivec4& color) const = 0;
	virtual void clear(const Texture& te, GLint level, const glm::uvec4& color) const = 0;
	virtual void clear(const Texture& te, GLint level, const Color& color) const = 0;

};

}
