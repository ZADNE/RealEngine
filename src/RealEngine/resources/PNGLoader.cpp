/**
 *  @author    Dubsky Tomas
 */
#include <lodepng/lodepng.hpp>

#include <RealEngine/resources/PNGLoader.hpp>
#include <RealEngine/utility/Endianness.hpp>
#include <RealEngine/utility/Error.hpp>

namespace re {

/**
 * @brief Represents erialized version of texture shape
 */
using RealTextureInformation =
    std::array<unsigned char, sizeof(uint32_t) + sizeof(glm::vec2) * 3>;

static TextureShape decodeTextureShape(const unsigned char* data, unsigned length) {
    TextureShape shape{};
    if (length >= sizeof(uint32_t)) {
        uint32_t version = ntoh(*reinterpret_cast<const uint32_t*>(data));
        length -= sizeof(uint32_t);
        switch (version) {
        case 0:
            if (length == sizeof(glm::vec2) * 3) {
                int i = 0;
                const float* fData =
                    reinterpret_cast<const float*>(&data[sizeof(version)]);
                shape.subimageDims.x          = ntoh(fData[i++]);
                shape.subimageDims.y          = ntoh(fData[i++]);
                shape.pivot.x                 = ntoh(fData[i++]);
                shape.pivot.y                 = ntoh(fData[i++]);
                shape.subimagesSpritesCount.x = ntoh(fData[i++]);
                shape.subimagesSpritesCount.y = ntoh(fData[i++]);
            }
            break;
        }
    }
    return shape;
}

static RealTextureInformation encodeTextureShape(const TextureShape& shape) {
    RealTextureInformation rti{};
    *reinterpret_cast<uint32_t*>(rti.data()) = hton<uint32_t>(0);
    float* v = reinterpret_cast<float*>(&rti[4]);
    int i    = 0;
    v[i++]   = hton(shape.subimageDims.x);
    v[i++]   = hton(shape.subimageDims.y);
    v[i++]   = hton(shape.pivot.x);
    v[i++]   = hton(shape.pivot.y);
    v[i++]   = hton(shape.subimagesSpritesCount.x);
    v[i++]   = hton(shape.subimagesSpritesCount.y);
    return rti;
}

PNGLoader::PNGData PNGLoader::load(const std::string& filePathPNG) {
    // Prepare variables
    lodepng::State state{};
    state.decoder.remember_unknown_chunks = 1;
    std::vector<unsigned char> encoded;
    unsigned int code{};
    PNGData decoded{};

    // Decode PNG
    if ((code = lodepng::load_file(encoded, filePathPNG)) ||
        (code = lodepng::decode(
             decoded.texels, decoded.dims.x, decoded.dims.y, state, encoded
         ))) {
        throw Exception{lodepng_error_text(code)}; // Loading or decoding failed
    }

    // Load parameters
    constexpr static size_t k_maxUnknownChunks =
        sizeof(LodePNGInfo::unknown_chunks_size) /
        sizeof(LodePNGInfo::unknown_chunks_size[0]);
    for (size_t i = 0; i < k_maxUnknownChunks; i++) {
        const auto& unknownData = state.info_png.unknown_chunks_data[i];
        const auto& unknownDataEnd =
            &unknownData[state.info_png.unknown_chunks_size[i]];
        for (auto chunk = unknownData; chunk != unknownDataEnd;
             chunk      = lodepng_chunk_next(chunk, unknownDataEnd)) {
            std::array<char, 5> type{}; // NOLINT(*-magic-numbers): 4 + null char
            lodepng_chunk_type(type.data(), chunk);
            if (std::string{"reAl"} == type.data()) {
                decoded.shape = decodeTextureShape(
                    lodepng_chunk_data_const(chunk), lodepng_chunk_length(chunk)
                );
                return decoded; // Successfully loaded RTI
            }
        }
    }

    return decoded; // Could not load RTI but that is still ok
}

void PNGLoader::save(const std::string& filePathPNG, const PNGData& data) {
    // Create RTI chunk
    lodepng::State state{};
    unsigned int code{};
    auto rti = encodeTextureShape(data.shape);
    if ((code = lodepng_chunk_create(
             &state.info_png.unknown_chunks_data[0],
             &state.info_png.unknown_chunks_size[0],
             static_cast<unsigned int>(rti.size()), "reAl", rti.data()
         ))) {
        throw Exception{lodepng_error_text(code)}; // Chunk creation failed
    }

    // Encode and save PNG
    state.info_png.color.colortype = LCT_RGBA;
    state.encoder.auto_convert     = 0;
    std::vector<unsigned char> png;
    if ((code = lodepng::encode(png, data.texels, data.dims.x, data.dims.y, state)) ||
        (code = lodepng::save_file(png, filePathPNG))) {
        throw Exception{lodepng_error_text(code)}; // Encoding or saving failed
    }
}

void PNGLoader::replaceParameters(
    const std::string& filePathPNG, const TextureShape& shape
) {
    auto saved  = load(filePathPNG);
    saved.shape = shape;
    save(filePathPNG, saved);
}

} // namespace re
