/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <span>
#include <string_view>

#include <RealEngine/graphics/batches/SpriteBatch.hpp>
#include <RealEngine/graphics/textures/Texture.hpp>

namespace re {

/**
 * @brief Denotes a range of Unicode characters
 */
struct UnicodeRange {
    char32_t firstChar{}; /**< Inclusive */
    char32_t lastChar{};  /**< Inclusive */
};

/**
 * @brief Range of ASCII characters that have printable glyph assigned
 */
constexpr UnicodeRange k_asciiPrintable{.firstChar = 32, .lastChar = 126};

struct RasterizedFontCreateInfo {
    const char* filePath{};
    int pointSize{};
    int faceIndex{};
    std::span<const UnicodeRange> ranges{&k_asciiPrintable, size_t{1}};
};

/**
 * @brief   Renders TrueType fonts
 * @details The glyphs are rasterized to a texture during construction.
 *          The actual rendering uses the rasterized glyphs.
 */
class RasterizedFont {
public:

    RasterizedFont(const RasterizedFontCreateInfo& createInfo);

    void add(SpriteBatch& batch, std::u8string_view str, glm::vec2 posPx) const;

private:

    struct Glyph {
        char32_t c;
        float advancePx;
        glm::vec2 sizePx;
        glm::vec4 uvSizeRect;
    };

    std::vector<Glyph> m_glyphs;
    Texture m_glyphTex;
};

} // namespace re
