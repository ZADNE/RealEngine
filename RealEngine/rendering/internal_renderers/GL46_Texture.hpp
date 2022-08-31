/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/ITexture.hpp>


namespace RE {

class Texture;
class TextureProxy;

/**
* @brief Is OpenGL 4.6 implementation of Texture class.
*
* Do not use this directly - use Texture instead.
*/
class GL46_Texture final : public ITexture {
public:

    void construct(Texture& te, const Raster& raster) const override;
    void destruct(Texture& te) const override;

    void setMinFilter(Texture& te, TextureMinFilter minFilter) const override;
    void setMagFilter(Texture& te, TextureMagFilter magFilter) const override;
    void setWrapStyleX(Texture& te, TextureWrapStyle wrapStyleX) const override;
    void setWrapStyleY(Texture& te, TextureWrapStyle wrapStyleY) const override;

    void setBorderColor(Texture& te, const glm::vec4& col) const override;

    void bind(const Texture& te) const override;
    void bind(const TextureProxy& te) const override;
    void bind(const Texture& te, TextureUnit unit) const override;
    void bind(const TextureProxy& te, TextureUnit unit) const override;
    void bindImage(const Texture& te, ImageUnit unit, int level, ImageAccess access) const override;

    void setTexels(const Texture& te, int level, const glm::ivec2& offset, const glm::ivec2& size, const void* raster) const override;
    void copyTexels(const Texture& te, int srcLevel, const glm::ivec2& srcPos, const Texture& destination, int dstLevel, const glm::ivec2& dstPos, const glm::ivec2& size) const override;
    void getTexels(const Texture& te, int level, const glm::ivec2& offset, const glm::ivec2& size, size_t bufSize, void* pixels) const override;

    void clear(const Texture& te, int level, const glm::vec4& color) const override;
    void clear(const Texture& te, int level, const glm::ivec4& color) const override;
    void clear(const Texture& te, int level, const glm::uvec4& color) const override;
    void clear(const Texture& te, int level, const Color& color) const override;

};

}
