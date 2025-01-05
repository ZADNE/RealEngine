/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>

#include <glm/vec2.hpp>

#include <RealEngine/graphics/textures/Texture.hpp>
#include <RealEngine/graphics/textures/TextureShape.hpp>
#include <RealEngine/resources/PNGLoader.hpp>

namespace re {

/**
 * @brief Is a 2D texture with associated shape, typically loaded from a PNG file
 */
class TextureShaped: public Texture {
public:
    /**
     * @brief Constructs a null Texture with no memory or images
     */
    explicit TextureShaped() = default;

    /**
     * @brief   Constructs texture from PNG
     * @param   filePathPNG Path to the PNG
     * @throws  Throws when the PNG cannot be loaded
     * @details Shape of the texture is also loaded from the texture,
     *          or if that cannot be done, default shape is used instead.
     */
    explicit TextureShaped(const std::string& filePathPNG)
        : TextureShaped(PNGLoader::load(filePathPNG)) {}

    /**
     * @brief   Constructs texture from already loaded PNG
     * @param   filePathPNG Path to the PNG
     * @throws  Throws when the PNG cannot be loaded
     */
    TextureShaped(const PNGLoader::PNGData& pngData);

    TextureShaped(const TextureShaped&)            = delete;  ///< Noncopyable
    TextureShaped& operator=(const TextureShaped&) = delete;  ///< Noncopyable

    TextureShaped(TextureShaped&& other) noexcept;            ///< Movable
    TextureShaped& operator=(TextureShaped&& other) noexcept; ///< Movable

    ~TextureShaped() = default;

    glm::vec2 subimageDims() const { return m_shape.subimageDims; }
    glm::vec2 pivot() const { return m_shape.pivot; }
    glm::vec2 subimagesSpritesCount() const {
        return m_shape.subimagesSpritesCount;
    }
    glm::uvec2 trueDims() const { return m_trueDims; }

    void setSubimageDims(glm::vec2 subimageDims) {
        m_shape.subimageDims = subimageDims;
    }
    void setPivot(glm::vec2 pivot) { m_shape.pivot = pivot; }
    void setSubimagesSpritesCount(glm::vec2 subimagesSpritesCount) {
        m_shape.subimagesSpritesCount = subimagesSpritesCount;
    }

private:

    TextureShape m_shape{};

    glm::uvec2 m_trueDims{};
};

} // namespace re
