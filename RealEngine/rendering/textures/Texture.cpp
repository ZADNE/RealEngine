/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/textures/Texture.hpp>

#include <RealEngine/rendering/textures/TextureFlagsToString.hpp>
#include <RealEngine/resources/PNGLoader.hpp>
#include <RealEngine/utility/Error.hpp>

#include <RealEngine/rendering/internal_renderers/GL46Texture.hpp>

namespace RE {

glm::vec4 colorToFloatColor(Color color, TextureFormat type) {
    glm::vec4 borderRGBA = glm::vec4{color.r, color.g, color.b, color.a};
    switch (type) {
    case TextureFormat::NORMALIZED_UNSIGNED: borderRGBA = borderRGBA / 255.0f; break;
    case TextureFormat::NORMALIZED_SIGNED: borderRGBA = (borderRGBA / 255.0f) * 2.0f - 1.0f; break;
    case TextureFormat::INTEGRAL_UNSIGNED: break;
    case TextureFormat::INTEGRAL_SIGNED: borderRGBA = borderRGBA - 128.0f; break;
    }
    return borderRGBA;
}

template<Renderer R>
TextureProxy<R>::TextureProxy(const Texture<R>& texture) :
    m_id(texture.m_id) {
}

template<Renderer R>
void TextureProxy<R>::bind() const {
    s_impl->bind(m_id);
}

template<Renderer R>
void TextureProxy<R>::bind(TextureUnit unit) const {
    s_impl->bind(m_id, unit);
}

template<Renderer R>
Texture<R>::Texture(Texture<R>&& other) noexcept :
    m_id(std::move(other.m_id)),
    m_flags(other.m_flags),
    m_subimageDims(other.m_subimageDims),
    m_pivot(other.m_pivot),
    m_subimagesSpritesCount(other.m_subimagesSpritesCount),
    m_trueDims(other.m_trueDims),
    m_borderColor(other.m_borderColor) {
}

template<Renderer R>
Texture<R>& Texture<R>::operator=(Texture<R>&& other) noexcept {
    m_id = std::move(other.m_id);
    m_flags = other.m_flags;
    m_trueDims = other.m_trueDims;
    m_subimageDims = other.m_subimageDims;
    m_pivot = other.m_pivot;
    m_subimagesSpritesCount = other.m_subimagesSpritesCount;
    m_borderColor = other.m_borderColor;
    return *this;
}

template<Renderer R>
Texture<R>::Texture(const std::string& filePathPNG) {
    PNGLoader::PNGData pngData{
        .params = DEFAULT_PARAMETERS
    };

    if (PNGLoader::load(filePathPNG, pngData) == 0) {
        init(Raster{pngData.dims, pngData.params.getChannels(), pngData.pixels}, pngData.params);
    }
}

template<Renderer R>
Texture<R>::Texture(const TextureSeed& seed) :
    Texture<R>(seed.toFullPath()) {
}

template<Renderer R>
Texture<R>::Texture(const Raster& raster, const TextureParameters& params/* = DEFAULT_PARAMETERS*/) {
    init(raster, params);
}

template<Renderer R>
Texture<R>::~Texture() {
    s_impl->destruct(m_id);
}

template<Renderer R>
TextureParameters Texture<R>::getParameters() const {
    return TextureParameters{TextureGeometry{m_subimageDims, m_pivot, m_subimagesSpritesCount}, m_flags, m_borderColor};
}

template<Renderer R>
void Texture<R>::setMinFilter(TextureMinFilter minFilter) {
    m_flags.setMinFilter(minFilter);
    s_impl->setMinFilter(m_id, minFilter);
}

template<Renderer R>
void Texture<R>::setMagFilter(TextureMagFilter magFilter) {
    m_flags.setMagFilter(magFilter);
    s_impl->setMagFilter(m_id, magFilter);
}

template<Renderer R>
void Texture<R>::setWrapStyleX(TextureWrapStyle wrapStyleX) {
    m_flags.setWrapStyleX(wrapStyleX);
    s_impl->setWrapStyleX(m_id, wrapStyleX);
}

template<Renderer R>
void Texture<R>::setWrapStyleY(TextureWrapStyle wrapStyleY) {
    m_flags.setWrapStyleY(wrapStyleY);
    s_impl->setWrapStyleY(m_id, wrapStyleY);
}

template<Renderer R>
void Texture<R>::setBorderColor(Color col) {
    m_borderColor = col;
    glm::vec4 borderRGBA = colorToFloatColor(col, getFormat());
    s_impl->setBorderColor(m_id, m_borderColor);
}

template<Renderer R>
void Texture<R>::setBorderColor(const glm::vec4& col) {
    m_borderColor = Color{col * 255.0f};
    s_impl->setBorderColor(m_id, col);
}

template<Renderer R>
void Texture<R>::bind() const {
    s_impl->bind(m_id);
}

template<Renderer R>
void Texture<R>::bind(TextureUnit unit) const {
    s_impl->bind(m_id, unit);
}

template<Renderer R>
void Texture<R>::bindImage(ImageUnit unit, int level, ImageAccess access) const {
    s_impl->bindImage(m_id, unit, level, access, m_flags);
}

template<Renderer R>
void Texture<R>::setTexels(int level, const glm::ivec2& offset, const glm::ivec2& size, const void* raster) const {
    s_impl->setTexels(m_id, level, offset, size, m_flags, raster);
}

template<Renderer R>
void Texture<R>::setTexels(const void* raster) const {
    s_impl->setTexels(m_id, 0, glm::ivec2(0, 0), glm::ivec2(getTrueDims()), m_flags, raster);
}

template<Renderer R>
void Texture<R>::copyTexels(int srcLevel, const glm::ivec2& srcPos, const Texture<R>& destination, int dstLevel, const glm::ivec2& dstPos, const glm::ivec2& size) const {
    s_impl->copyTexels(m_id, srcLevel, srcPos, destination.m_id, dstLevel, dstPos, size);
}

template<Renderer R>
void Texture<R>::getTexels(int level, const glm::ivec2& offset, const glm::ivec2& size, size_t bufSize, void* buffer) {
    s_impl->getTexels(m_id, level, offset, size, m_flags, bufSize, buffer);
}

template<Renderer R>
void Texture<R>::getTexels(size_t bufSize, void* buffer) {
    s_impl->getTexels(m_id, 0, glm::ivec2(0, 0), glm::ivec2(getTrueDims()), m_flags, bufSize, buffer);
}

template<Renderer R>
void Texture<R>::clear(const glm::vec4& color) const {
    s_impl->clear(m_id, 0, color);
}

template<Renderer R>
void Texture<R>::clear(const glm::ivec4& color) const {
    s_impl->clear(m_id, 0, color);
}

template<Renderer R>
void Texture<R>::clear(const glm::uvec4& color) const {
    s_impl->clear(m_id, 0, color);
}

template<Renderer R>
void Texture<R>::clear(Color color) const {
    s_impl->clear(m_id, 0, color);
}

template<Renderer R>
bool Texture<R>::saveToFile(const std::string& filePathPNG) {
    return saveToFile(filePathPNG, getParameters());
}

template<Renderer R>
bool Texture<R>::saveToFile(const std::string& filePathPNG, const TextureParameters& params) {
    PNGLoader::PNGData pngData{
        .dims = m_trueDims,
        .params = params
    };
    pngData.pixels.resize(Raster::minimumRequiredMemory(m_trueDims, params.getChannels()));
    //Download pixels
    getTexels(pngData.pixels.size(), pngData.pixels.data());
    return PNGLoader::save(filePathPNG, pngData) == 0;
}

template<Renderer R>
void Texture<R>::init(const Raster& raster, const TextureParameters& params) {
    if (params.isGeometryDefinedByImage()) {//Geometry defined by image
        m_subimageDims = raster.getDims();
        m_pivot = glm::vec2{0.0f, 0.0f};
        m_subimagesSpritesCount = glm::vec2{1.0f, 1.0f};
    } else {//Geometry defined by parameters
        m_subimageDims = params.getSubimageDims();
        m_pivot = params.getPivot();
        m_subimagesSpritesCount = params.getSubimagesSpritesCount();
    }
    m_flags = TextureFlags{params};
    m_trueDims = raster.getDims();
    m_borderColor = params.getBorderColor();

    m_id = s_impl->construct(m_flags, raster);

    //Set parameters of the texture
    setMinFilter(getMinFilter());
    setMagFilter(getMagFilter());
    setWrapStyleX(getWrapStyleX());
    setWrapStyleY(getWrapStyleY());
    setBorderColor(getBorderColor());
}

template TextureProxy<RendererLateBind>;
template TextureProxy<RendererGL46>;
template Texture<RendererLateBind>;
template Texture<RendererGL46>;

}