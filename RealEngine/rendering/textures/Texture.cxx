/*!
 *  @author    Dubsky Tomas
 */
module RealEngine.rendering.textures.Texture;


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
bool Texture<R>::saveToFile(const std::string& filePathPNG) {
    return saveToFile(filePathPNG, getParameters());
}

template<Renderer R>
bool Texture<R>::saveToFile(const std::string& filePathPNG, const TextureParameters& params) {
    auto channels = params.getChannels();
    //Download pixels
    std::vector<unsigned char> pixels;
    pixels.resize(Raster::minimumRequiredMemory(m_trueDims, channels));
    getTexels(pixels.size(), pixels.data());

    //Insert rti chunk
    lodepng::State state{};
    auto rti = params.convertToRTI();
    if (lodepng_chunk_create(&state.info_png.unknown_chunks_data[0], &state.info_png.unknown_chunks_size[0], static_cast<unsigned int>(rti.size()), "reAl", rti.data())) {
        //Chunk creation failed
        return false;
    }

    //Encode and save PNG
    unsigned int code;
    state.info_png.color.colortype = toLodePNGColorType(channels);
    state.encoder.auto_convert = 0;
    std::vector<unsigned char> png;
    if ((code = lodepng::encode(png, pixels, m_trueDims.x, m_trueDims.y, state)) || (code = lodepng::save_file(png, filePathPNG))) {
        //Encoding or saving failed
        return false;
    }

    return true;
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