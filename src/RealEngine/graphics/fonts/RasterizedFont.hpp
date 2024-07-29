/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <span>
#include <string_view>

#include <RealEngine/graphics/batches/SpriteBatch.hpp>
#include <RealEngine/graphics/textures/Texture.hpp>
#include <RealEngine/utility/Unicode.hpp>

namespace re {

struct RasterizedFontCreateInfo {
    const char* filePath{};
    int pointSize{};
    int faceIndex{};
    std::span<const UnicodeRange> ranges = k_asciiPrintableUnicodeRanges;
};

/**
 * @brief   Renders TrueType fonts
 * @details The glyphs are rasterized to a texture during construction.
 *          The actual rendering uses the rasterized glyphs.
 */
class RasterizedFont {
public:

    RasterizedFont(const RasterizedFontCreateInfo& createInfo);

    /**
     * @brief Adds text str to batch at posPx
     * @note  It is undefined behavior if str contains a character that wes not
     *        in a range when the Font was constructed
     */
    void add(SpriteBatch& batch, std::u8string_view str, glm::vec2 posPx) const;

private:

    /**
     * @brief Translates codes to indices into m_glyphs
     */
    int codeToOffset(char32_t c) const;

    struct GlyphOffset {
        char32_t lastChar{}; // If c is less-or-equal than this
        int baseOffset{};    // c is at: baseOffset - (lastChar - c)
    };
    std::vector<GlyphOffset> m_offsets;

    struct Glyph {
        glm::vec4 uvSizeRect;
        glm::vec2 sizePx;
        float advancePx;
    };
    std::vector<Glyph> m_glyphs;

    Texture m_glyphTex;

    float m_lineSkipPx{};
};

} // namespace re
