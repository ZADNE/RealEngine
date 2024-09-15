/*!
 *  @author    Dubsky Tomas
 */
#include <bit>
#include <format>
#include <memory>
#include <numeric>

#include <SDL_ttf.h>

#include <RealEngine/graphics/fonts/RasterizedFont.hpp>
#include <RealEngine/utility/Error.hpp>
#include <RealEngine/utility/Math.hpp>
#include <RealEngine/utility/Unicode.hpp>
#include <RealEngine/utility/UniqueCPtr.hpp>

namespace re {

using TTF_FontRAII    = UniqueCPtr<TTF_Font, TTF_CloseFont>;
using SDL_SurfaceRAII = UniqueCPtr<SDL_Surface, SDL_FreeSurface>;

RasterizedFont::RasterizedFont(const RasterizedFontCreateInfo& createInfo) {
    TTF_FontRAII font{TTF_OpenFontIndex(
        createInfo.filePath, createInfo.pointSize, createInfo.faceIndex
    )};
    if (font == nullptr) {
        throw Exception{std::format("Could not load font {}", createInfo.filePath)};
    }

    // Count the expected number of characters
    int glyphCount = std::accumulate(
        createInfo.ranges.begin(), createInfo.ranges.end(), 0,
        [](int c, UnicodeRange r) { return c + (r.lastChar - r.firstChar + 1); }
    );

    // Reserve space
    m_offsets.reserve(glyphCount);
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
            const SDL_SurfaceRAII& surf = surfs.emplace_back(
                TTF_RenderGlyph32_Blended(font.get(), c, k_fgCol)
            );
            if (surf) { // If there is a glyph for the character
                totalWidth += surf->w;
                int advance{};
                TTF_GlyphMetrics32(
                    font.get(), c, &dontCare, &dontCare, &dontCare, &dontCare, &advance
                );
                m_glyphs.emplace_back(
                    glm::vec4{}, glm::vec2{surf->w, surf->h},
                    static_cast<float>(advance)
                );
            } else {
                m_glyphs.emplace_back();
            }
        }
        m_offsets.emplace_back(r.lastChar, static_cast<int>(m_glyphs.size()) - 1);
    }
    int height   = TTF_FontHeight(font.get());
    m_ascentPx   = static_cast<float>(TTF_FontAscent(font.get()));
    m_lineSkipPx = static_cast<float>(TTF_FontLineSkip(font.get()));

    // Calculate size of the texture and prepare stage
    const uint32_t minArea = totalWidth * height;
    const uint32_t squareRoot =
        static_cast<uint32_t>(std::sqrt(static_cast<float>(minArea)));
    const uint32_t texWidth = std::bit_ceil(squareRoot);
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
        if (const SDL_Surface* surf = surfs[i].get()) {
            auto& glyph = m_glyphs[i];
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
    SpriteBatch& batch, std::u8string_view str, glm::vec2 anchorPx,
    Color col /* = SpriteBatch::k_white*/
) const {
    anchorPx.y += m_ascentPx;
    auto alignLeft = [this, &anchorPx]([[maybe_unused]] std::u8string_view str) {
        anchorPx.y -= m_lineSkipPx;
        return anchorPx;
    };
    addGeneric(batch, str, alignLeft, col);
}

void RasterizedFont::add(
    SpriteBatch& batch, std::u8string_view str, glm::vec2 anchorPx,
    HorAlign horAlign, Color col /* = SpriteBatch::k_white*/
) const {
    switch (horAlign) {
    case HorAlign::Left: add(batch, str, anchorPx); break;
    case HorAlign::Center:
    case HorAlign::Right:
        anchorPx.y += m_ascentPx;
        float factor = static_cast<float>(horAlign) * 0.5f;
        auto align   = [this, &anchorPx, factor](std::u8string_view str) {
            anchorPx.y -= m_lineSkipPx;
            float lineWidth = measureLineWidth(str);
            return glm::vec2{anchorPx.x - factor * lineWidth, anchorPx.y};
        };
        addGeneric(batch, str, align, col);
        break;
    }
}

int RasterizedFont::codeToIndex(char32_t c) const {
    for (const GlyphOffset& offset : m_offsets) {
        if (offset.lastChar >= c) {
            return offset.baseOffset - (offset.lastChar - c); // NOLINT(*-conversions)
        }
    }
    assert(!"Character not found");
    return 0;
}

float RasterizedFont::measureLineWidth(std::u8string_view str) const {
    auto nonEndingChar = [](char32_t c) { return (c != U'\0' && c != U'\n'); };
    float widthPx      = 0.0f;
    char32_t prevC     = readCode(str);
    if (nonEndingChar(prevC)) { // If not 0 char line
        char32_t c{};
        while (nonEndingChar(c = readCode(str))) {
            widthPx += m_glyphs[codeToIndex(prevC)].advancePx;
            prevC = c;
        };
    }
    widthPx += m_glyphs[codeToIndex(prevC)].sizePx.x;
    return widthPx;
}

template<std::invocable<std::u8string_view> AlignFunc>
void RasterizedFont::addGeneric(
    SpriteBatch& batch, std::u8string_view str, const AlignFunc& align, Color col
) const {
    glm::vec2 cursorPx = align(str);
    while (!str.empty()) {
        char32_t c = readCode(str);
        switch (c) {
        case U'\n':         cursorPx = align(str); break;
        case k_invalidCode: break;
        default:
            const Glyph& glyph = m_glyphs[codeToIndex(c)];
            batch.add(
                m_glyphTex, glm::vec4{cursorPx, glyph.sizePx}, glyph.uvSizeRect, col
            );
            cursorPx.x += glyph.advancePx;
            break;
        }
    }
}

} // namespace re
