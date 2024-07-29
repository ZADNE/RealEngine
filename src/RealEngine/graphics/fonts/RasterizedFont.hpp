/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <concepts>
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
 * @brief Describes horizontal alignment of text
 */
enum class HorAlign {
    Left,
    Center,
    Right
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
     * @brief Adds text str to batch at anchorPx
     * @note  It is undefined behavior if str contains a character that was not
     *        in a range when the Font was constructed
     */
    void add(SpriteBatch& batch, std::u8string_view str, glm::vec2 anchorPx) const;

    /**
     * @brief Adds text str to batch at anchorPx, aligned according to horAlign
     */
    void add(
        SpriteBatch& batch, std::u8string_view str, glm::vec2 anchorPx, HorAlign horAlign
    ) const;

private:

    /**
     * @brief Translates codes to indices into m_glyphs
     */
    int codeToIndex(char32_t c) const;

    /**
     * @brief Measures width (in pixels) of the first line in str
     */
    float measureLineWidth(std::u8string_view str) const;

    template<std::invocable<std::u8string_view> AlignFunc>
    void addGeneric(SpriteBatch& batch, std::u8string_view str, AlignFunc&& align) const;

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
    float m_ascentPx{};
};

} // namespace re
