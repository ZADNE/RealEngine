﻿/*!
 *  @author    Dubsky Tomas
 */
#include <bit>
#include <format>
#include <memory>
#include <numeric>

#include <SDL2/SDL_ttf.h>
#include <glm/vec2.hpp>

#include <RealEngine/graphics/fonts/RasterizedFont.hpp>
#include <RealEngine/utility/Error.hpp>
#include <RealEngine/utility/Math.hpp>
#include <RealEngine/utility/RAIIWrapper.hpp>
#include <RealEngine/utility/Unicode.hpp>

namespace re {

using TTF_FontRAII    = RAIIWrapper<TTF_Font, decltype(TTF_CloseFont)*>;
using SDL_SurfaceRAII = RAIIWrapper<SDL_Surface, decltype(SDL_FreeSurface)*>;

RasterizedFont::RasterizedFont(const RasterizedFontCreateInfo& createInfo) {
    TTF_FontRAII font = TTF_OpenFontIndex(
        createInfo.filePath, createInfo.pointSize, createInfo.faceIndex
    );
    if (font == nullptr) {
        throw Exception{std::format("Could not load font {}", createInfo.filePath)};
    }

    // Count the expected number of characters
    int glyphCount = std::accumulate(
        createInfo.ranges.begin(), createInfo.ranges.end(), 0,
        [](int c, UnicodeRange r) { return c + (r.lastChar - r.firstChar); }
    );

    // Reserve space
    m_glyphs.reserve(glyphCount);
    std::vector<SDL_SurfaceRAII> surfs;
    surfs.reserve(glyphCount);

    // Measure and render all glyphs
    constexpr SDL_Color k_fgCol{.r = 255, .g = 255, .b = 255, .a = 255};
    int totalWidth = 0;
    [[maybe_unused]] int dontCare{};
    for (const UnicodeRange& r : createInfo.ranges) {
        assert(r.firstChar <= r.lastChar);
        for (char32_t c = r.firstChar; c <= r.lastChar; ++c) {
            if (TTF_GlyphIsProvided32(font, c)) {
                surfs.emplace_back(TTF_RenderGlyph32_Blended(font, c, k_fgCol));
                const SDL_Surface* surf = surfs.back();
                totalWidth += surf->w;
                int advance{};
                TTF_GlyphMetrics32(
                    font, c, &dontCare, &dontCare, &dontCare, &dontCare, &advance
                );
                m_glyphs.emplace_back(
                    c, static_cast<float>(advance), glm::vec2{surf->w, surf->h},
                    glm::vec4{}
                );
            } else {
                throw Exception{std::format(
                    "Font {} does not contain glyph for character at UNICODE "
                    "index {}",
                    createInfo.filePath, static_cast<int>(c)
                )};
            }
        }
    }
    int height = TTF_FontHeight(font);

    // Calculate size of the texture and prepare stage
    const uint32_t minArea    = totalWidth * height;
    const uint32_t squareRoot = std::sqrt(static_cast<float>(minArea));
    const uint32_t texWidth   = std::bit_ceil(squareRoot);
    const uint32_t texHeight =
        roundToMultiple(ceilDiv(minArea, texWidth) + height, height);
    const uint32_t area = texWidth * texHeight;
    std::vector<unsigned char> stage;
    stage.resize(area);

    // Lambda for copying from SDL surface to stage buffer
    auto copyGlyph = [stage = stage.data(),
                      &texWidth](const SDL_Surface* surf, glm::ivec2 botLeft) {
        const int rowCopyBytes = surf->w;
        const int pitchBytes   = surf->pitch;
        const auto pixels      = reinterpret_cast<unsigned char*>(surf->pixels);
        for (int yGlyph = 0; yGlyph < surf->h; yGlyph++) {
            int y = yGlyph + botLeft.y;
            for (int xGlyph = 0; xGlyph < surf->w; xGlyph++) {
                int x = xGlyph + botLeft.x;
                // Extract alpha channel only
                stage[x + y * texWidth] =
                    pixels[(xGlyph * 4 + yGlyph * pitchBytes) + 3];
            }
        }
    };

    // Copy glyphs to the texture
    glm::vec2 texSizeInv = 1.0f / glm::vec2{texWidth, texHeight};
    glm::ivec2 botLeft   = glm::ivec2{0, 0};
    for (int i = 0; i < glyphCount; i++) {
        const SDL_Surface* surf = surfs[i];
        auto& glyph             = m_glyphs[i];
        if (botLeft.x + surf->w >= texWidth) {           // If would not fit
            botLeft = glm::ivec2{0, botLeft.y + height}; // Jump to next row
            assert(botLeft.y <= texHeight);
        }
        copyGlyph(surf, botLeft);
        glyph.uvSizeRect = glm::vec4{
            glm::vec2{botLeft.x, texHeight - botLeft.y - height - 1} * texSizeInv,
            glyph.sizePx * texSizeInv
        };
        botLeft.x += surf->w;
    }

    // Create the final texture
    using enum vk::ComponentSwizzle;
    m_glyphTex = Texture{TextureCreateInfo{
        .format           = vk::Format::eR8Unorm,
        .extent           = glm::uvec3{texWidth, texHeight, 1},
        .componentMapping = {eOne, eOne, eOne, eR},
        .magFilter        = vk::Filter::eLinear,
        .minFilter        = vk::Filter::eLinear,
        .texels           = stage
    }};
}

void RasterizedFont::add(
    SpriteBatch& batch, std::u8string_view str, glm::vec2 posPx
) const {
    while (!str.empty()) {
        char32_t c = readCode(str);
        if (c != k_invalidCode) {
            const Glyph& glyph = m_glyphs[c - 32];
            batch.add(m_glyphTex, glm::vec4{posPx, glyph.sizePx}, glyph.uvSizeRect);
            posPx.x += glyph.advancePx;
        }
    }
}

} // namespace re
