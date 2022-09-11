/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <compare>

#include <RealEngine/rendering/textures/Raster.hpp>
#include <RealEngine/rendering/textures/TextureUnit.hpp>
#include <RealEngine/rendering/textures/ImageUnit.hpp>


namespace RE {

template<Renderer> class TextureProxy;

/**
 * @brief Contains all members of Texture
 * @note For internal usage in RealEngine.
*/
class TextureID {
    template<Renderer> friend class Texture;
    template<Renderer> friend class TextureProxy;
    friend class GL46Texture;
    friend class GL46Framebuffer;
public:

    TextureID() : m_id(0) {}

    ~TextureID() = default;

    TextureID(TextureID&& other) noexcept :
        m_id(other.m_id) {
        other.m_id = 0;
    }

    TextureID& operator=(const TextureID&) = delete;
    TextureID& operator=(TextureID&& other) noexcept {
        std::swap(m_id, other.m_id);
        return *this;
    }

    auto operator<=>(const TextureID&) const = default;

private:

    TextureID(unsigned int id) :
        m_id(id) {}

    //This is used by TextureProxy
    TextureID(const TextureID& other) noexcept :
        m_id(other.m_id) {
    }

    unsigned int m_id = 0;      /**< Internal identifier */
};

/**
* @brief Is a renderer-agnostic interface to Texture's implementation.
*
* This is used internally by Texture.
*
* @see RE::Texture
*/
class ITexture {
public:

    virtual TextureID construct(TextureFlags flags, const Raster& raster) const = 0;
    virtual void destruct(TextureID& te) const = 0;

    virtual void setMinFilter(TextureID& te, TextureMinFilter minFilter) const = 0;
    virtual void setMagFilter(TextureID& te, TextureMagFilter magFilter) const = 0;
    virtual void setWrapStyleX(TextureID& te, TextureWrapStyle wrapStyleX) const = 0;
    virtual void setWrapStyleY(TextureID& te, TextureWrapStyle wrapStyleY) const = 0;

    virtual void setBorderColor(TextureID& te, const glm::vec4& col) const = 0;

    virtual void bind(const TextureID& te) const = 0;
    virtual void bind(const TextureID& te, TextureUnit unit) const = 0;
    virtual void bindImage(const TextureID& te, ImageUnit unit, int level, ImageAccess access, TextureFlags flags) const = 0;

    virtual void setTexels(const TextureID& te, int level, const glm::ivec2& offset, const glm::ivec2& size, const void* raster) const = 0;
    virtual void copyTexels(const TextureID& te, int srcLevel, const glm::ivec2& srcPos, const TextureID& destination, int dstLevel, const glm::ivec2& dstPos, const glm::ivec2& size) const = 0;
    virtual void getTexels(const TextureID& te, int level, const glm::ivec2& offset, const glm::ivec2& size, size_t bufSize, void* pixels) const = 0;

    virtual void clear(const TextureID& te, int level, const glm::vec4& color) const = 0;
    virtual void clear(const TextureID& te, int level, const glm::ivec4& color) const = 0;
    virtual void clear(const TextureID& te, int level, const glm::uvec4& color) const = 0;
    virtual void clear(const TextureID& te, int level, const Color& color) const = 0;
};

}
