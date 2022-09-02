/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <compare>

#include <RealEngine/rendering/textures/Raster.hpp>
#include <RealEngine/rendering/textures/TextureUnit.hpp>
#include <RealEngine/rendering/textures/ImageUnit.hpp>


namespace RE {

template<typename> class TextureProxy;

/**
 * @brief Contains all members of Texture
 * @note For internal usage in RealEngine.
*/
class TextureInternals {
    template<typename> friend class Texture;
    template<typename> friend class TextureProxy;
    friend class GL46_Texture;
    friend class GL46_Framebuffer;
public:

    TextureInternals() : m_id(0) {}

    ~TextureInternals() = default;

    TextureInternals(TextureInternals&& other) noexcept :
        m_id(other.m_id) {
        other.m_id = 0;
    }

    TextureInternals& operator=(const TextureInternals&) = delete;
    TextureInternals& operator=(TextureInternals&& other) noexcept {
        std::swap(m_id, other.m_id);
        return *this;
    }

    auto operator<=>(const TextureInternals&) const = default;

private:

    TextureInternals(unsigned int id) :
        m_id(id) {}

    //This is used by TextureProxy
    TextureInternals(const TextureInternals& other) noexcept :
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

    virtual TextureInternals construct(TextureFlags flags, const Raster& raster) const = 0;
    virtual void destruct(TextureInternals& te) const = 0;

    virtual void setMinFilter(TextureInternals& te, TextureMinFilter minFilter) const = 0;
    virtual void setMagFilter(TextureInternals& te, TextureMagFilter magFilter) const = 0;
    virtual void setWrapStyleX(TextureInternals& te, TextureWrapStyle wrapStyleX) const = 0;
    virtual void setWrapStyleY(TextureInternals& te, TextureWrapStyle wrapStyleY) const = 0;

    virtual void setBorderColor(TextureInternals& te, const glm::vec4& col) const = 0;

    virtual void bind(const TextureInternals& te) const = 0;
    virtual void bind(const TextureInternals& te, TextureUnit unit) const = 0;
    virtual void bindImage(const TextureInternals& te, ImageUnit unit, int level, ImageAccess access, TextureFlags flags) const = 0;

    virtual void setTexels(const TextureInternals& te, int level, const glm::ivec2& offset, const glm::ivec2& size, const void* raster) const = 0;
    virtual void copyTexels(const TextureInternals& te, int srcLevel, const glm::ivec2& srcPos, const TextureInternals& destination, int dstLevel, const glm::ivec2& dstPos, const glm::ivec2& size) const = 0;
    virtual void getTexels(const TextureInternals& te, int level, const glm::ivec2& offset, const glm::ivec2& size, size_t bufSize, void* pixels) const = 0;

    virtual void clear(const TextureInternals& te, int level, const glm::vec4& color) const = 0;
    virtual void clear(const TextureInternals& te, int level, const glm::ivec4& color) const = 0;
    virtual void clear(const TextureInternals& te, int level, const glm::uvec4& color) const = 0;
    virtual void clear(const TextureInternals& te, int level, const Color& color) const = 0;
};

}
