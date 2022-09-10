﻿/*!
 *  @author    Dubsky Tomas
 */
module RealEngine.rendering.textures.PNGLoader;
import RealEngine.utility.Error;


namespace RE {

LodePNGColorType toLodePNGColorType(TextureChannels channels) {
    static_assert(TEX_CHANNELS_BITS == 0b0000'0000'0000'0011, "TextureFlags - channel bits reaaranged");
    static const LodePNGColorType colorTypes[] = {
        LodePNGColorType::LCT_GREY, LodePNGColorType::LCT_GREY_ALPHA, LodePNGColorType::LCT_RGB, LodePNGColorType::LCT_RGBA
    };
    return colorTypes[ft_cast(channels)];
}

TextureChannels toTextureChannels(LodePNGColorType colorType) {
    switch (colorType) {
    case LCT_GREY: return TextureChannels::R;
    case LCT_RGB: return TextureChannels::RGB;
    case LCT_GREY_ALPHA: return TextureChannels::RG;
    case LCT_RGBA: return TextureChannels::RGBA;
    default: return TextureChannels::RGBA;
    }
}

unsigned int PNGLoader::load(const std::string& filePathPNG, LoadedData& tex) {
    //Prepare variables
    lodepng::State state{};
    state.decoder.remember_unknown_chunks = 1;
    std::vector<unsigned char> png;
    unsigned int code;

    //Decode PNG
    if ((code = lodepng::load_file(png, filePathPNG)) || (code = lodepng::decode(tex.pixels, tex.dims.x, tex.dims.y, state, png))) {
        error(filePathPNG + ": " + lodepng_error_text(code));
        return code;//Loading or decoding failed
    }
    tex.channels = toTextureChannels(state.info_raw.colortype);

    //Load parameters
    for (size_t unknownChunkPos = 0; unknownChunkPos < 3; unknownChunkPos++) {
        const auto& unknownData = state.info_png.unknown_chunks_data[0];
        const auto& unknownDataEnd = &unknownData[state.info_png.unknown_chunks_size[0]];
        for (auto chunk = unknownData; chunk != unknownDataEnd; chunk = lodepng_chunk_next(chunk, unknownDataEnd)) {
            char type[5];
            lodepng_chunk_type(type, chunk);
            if (std::string{"reAl"} == type) {
                try {
                    params = TextureParameters{lodepng_chunk_data(chunk), lodepng_chunk_length(chunk)};
                    return 0;//Successfully loaded RTI
                }
                catch (const char* e) {
                    error(std::string{"RTI decode fail: "} + e);
                }
            }
        }
    }

    return 0;//Could not load RTI but that is still ok
}

}