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
class GL46_Texture final : public ITexture {
public:

    TextureInternals construct(TextureFlags flags, const Raster& raster) const override;
    void destruct(TextureInternals& te) const override;

    void setMinFilter(TextureInternals& te, TextureMinFilter minFilter) const override;
    void setMagFilter(TextureInternals& te, TextureMagFilter magFilter) const override;
    void setWrapStyleX(TextureInternals& te, TextureWrapStyle wrapStyleX) const override;
    void setWrapStyleY(TextureInternals& te, TextureWrapStyle wrapStyleY) const override;

    void setBorderColor(TextureInternals& te, const glm::vec4& col) const override;

    void bind(const TextureInternals& te) const override;
    void bind(const TextureInternals& te, TextureUnit unit) const override;
    void bindImage(const TextureInternals& te, ImageUnit unit, int level, ImageAccess access, TextureFlags flags) const override;

    void setTexels(const TextureInternals& te, int level, const glm::ivec2& offset, const glm::ivec2& size, const void* raster) const override;
    void copyTexels(const TextureInternals& te, int srcLevel, const glm::ivec2& srcPos, const TextureInternals& destination, int dstLevel, const glm::ivec2& dstPos, const glm::ivec2& size) const override;
    void getTexels(const TextureInternals& te, int level, const glm::ivec2& offset, const glm::ivec2& size, size_t bufSize, void* pixels) const override;

    void clear(const TextureInternals& te, int level, const glm::vec4& color) const override;
    void clear(const TextureInternals& te, int level, const glm::ivec4& color) const override;
    void clear(const TextureInternals& te, int level, const glm::uvec4& color) const override;
    void clear(const TextureInternals& te, int level, const Color& color) const override;
};

}
