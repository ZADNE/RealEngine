/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/resources/PNGLoader.hpp>

#include <lodepng/lodepng.hpp>

#include <RealEngine/utility/Endianness.hpp>
#include <RealEngine/utility/Error.hpp>

namespace RE {

using RTI = std::array<unsigned char, sizeof(uint32_t) + sizeof(glm::vec2) * 3>;

TextureShape decodeTextureShape(const unsigned char* data, unsigned length) {
    TextureShape shape{};
    if (length >= sizeof(uint32_t)) {
        uint32_t version = ntoh(*reinterpret_cast<const uint32_t*>(data));
        length -= sizeof(uint32_t);
        switch (version) {
        case 0:
            if (length == sizeof(glm::vec2) * 3) {
                int i = 0;
                const float* fData = reinterpret_cast<const float*>(&data[sizeof(version)]);
                shape.subimageDims.x = ntoh(fData[i++]);
                shape.subimageDims.y = ntoh(fData[i++]);
                shape.pivot.x = ntoh(fData[i++]);
                shape.pivot.y = ntoh(fData[i++]);
                shape.subimagesSpritesCount.x = ntoh(fData[i++]);
                shape.subimagesSpritesCount.y = ntoh(fData[i++]);
            }
            break;
        }
    }
    return shape;
}

RTI encodeTextureShape(const TextureShape& shape) {
    RTI rti{};
    *reinterpret_cast<uint32_t*>(rti.data()) = hton<uint32_t>(0);
    float* v = reinterpret_cast<float*>(&rti[4]);
    int i = 0;
    v[i++] = hton(shape.subimageDims.x);
    v[i++] = hton(shape.subimageDims.y);
    v[i++] = hton(shape.pivot.x);
    v[i++] = hton(shape.pivot.y);
    v[i++] = hton(shape.subimagesSpritesCount.x);
    v[i++] = hton(shape.subimagesSpritesCount.y);
    return rti;
}

PNGLoader::PNGData PNGLoader::load(const std::string& filePathPNG) {
    //Prepare variables
    lodepng::State state{};
    state.decoder.remember_unknown_chunks = 1;
    std::vector<unsigned char> encoded;
    unsigned int code;
    PNGData decoded{};

    //Decode PNG
    if ((code = lodepng::load_file(encoded, filePathPNG)) || (code = lodepng::decode(decoded.texels, decoded.dims.x, decoded.dims.y, state, encoded))) {
        throw Exception{lodepng_error_text(code)};//Loading or decoding failed
    }

    //Load parameters
    for (size_t unknownChunkPos = 0; unknownChunkPos < 3; unknownChunkPos++) {
        const auto& unknownData = state.info_png.unknown_chunks_data[0];
        const auto& unknownDataEnd = &unknownData[state.info_png.unknown_chunks_size[0]];
        for (auto chunk = unknownData; chunk != unknownDataEnd; chunk = lodepng_chunk_next(chunk, unknownDataEnd)) {
            char type[5];
            lodepng_chunk_type(type, chunk);
            if (std::string{"reAl"} == type) {
                decoded.shape = decodeTextureShape(lodepng_chunk_data_const(chunk), lodepng_chunk_length(chunk));
                return decoded;//Successfully loaded RTI
            }
        }
    }

    return decoded;//Could not load RTI but that is still ok
}

void PNGLoader::save(const std::string& filePathPNG, const PNGData& data) {
    //Create RTI chunk
    lodepng::State state{};
    unsigned int code;
    auto rti = encodeTextureShape(data.shape);
    if (code = lodepng_chunk_create(
        &state.info_png.unknown_chunks_data[0],
        &state.info_png.unknown_chunks_size[0],
        static_cast<unsigned int>(rti.size()),
        "reAl", rti.data())) {
        throw Exception{lodepng_error_text(code)};//Chunk creation failed
    }

    //Encode and save PNG
    state.info_png.color.colortype = LCT_RGBA;
    state.encoder.auto_convert = 0;
    std::vector<unsigned char> png;
    if ((code = lodepng::encode(png, data.texels, data.dims.x, data.dims.y, state)) || (code = lodepng::save_file(png, filePathPNG))) {
        throw Exception{lodepng_error_text(code)};//Encoding or saving failed
    }
}

void PNGLoader::replaceParameters(const std::string& filePathPNG, const TextureShape& shape) {
    auto saved = load(filePathPNG);
    saved.shape = shape;
    save(filePathPNG, saved);
}

}