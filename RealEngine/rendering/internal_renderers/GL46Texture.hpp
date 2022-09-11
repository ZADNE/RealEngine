/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/ITexture.hpp>


namespace RE {

/**
* @brief Is OpenGL 4.6 implementation of Texture class.
*
* Do not use this directly - use Texture instead.
*/
class GL46Texture final : public ITexture {
public:

    TextureID construct(TextureFlags flags, const Raster& raster) const override;
    void destruct(TextureID& te) const override;

    void setMinFilter(TextureID& te, TextureMinFilter minFilter) const override;
    void setMagFilter(TextureID& te, TextureMagFilter magFilter) const override;
    void setWrapStyleX(TextureID& te, TextureWrapStyle wrapStyleX) const override;
    void setWrapStyleY(TextureID& te, TextureWrapStyle wrapStyleY) const override;

    void setBorderColor(TextureID& te, const glm::vec4& col) const override;

    void bind(const TextureID& te) const override;
    void bind(const TextureID& te, TextureUnit unit) const override;
    void bindImage(const TextureID& te, ImageUnit unit, int level, ImageAccess access, TextureFlags flags) const override;

    void setTexels(const TextureID& te, int level, const glm::ivec2& offset, const glm::ivec2& size, TextureFlags flags, const void* raster) const override;
    void copyTexels(const TextureID& te, int srcLevel, const glm::ivec2& srcPos, const TextureID& destination, int dstLevel, const glm::ivec2& dstPos, const glm::ivec2& size) const override;
    void getTexels(const TextureID& te, int level, const glm::ivec2& offset, const glm::ivec2& size, TextureFlags flags, size_t bufSize, void* pixels) const override;

    void clear(const TextureID& te, int level, const glm::vec4& color) const override;
    void clear(const TextureID& te, int level, const glm::ivec4& color) const override;
    void clear(const TextureID& te, int level, const glm::uvec4& color) const override;
    void clear(const TextureID& te, int level, const Color& color) const override;
};

}
