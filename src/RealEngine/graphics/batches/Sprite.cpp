/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/batches/Sprite.hpp>

namespace re {

SpriteStatic::SpriteStatic(
    const TextureShaped& tex, float sprite, float subimage /* = 0.0f*/
)
    : m_tex(&tex)
    , m_subimageSprite(subimage, sprite) {
}

float SpriteStatic::speed() const {
    return 1.0f;
}

Color SpriteStatic::color() const {
    return Color{255u, 255u, 255u, 255u};
}

glm::vec2 SpriteStatic::scale() const {
    return glm::vec2(1.0f, 1.0f);
}

glm::vec2 SpriteStatic::subimageSprite() const {
    return m_subimageSprite;
}

void SpriteStatic::step() {
}

SpriteAnimated::SpriteAnimated(
    const TextureShaped& tex, float sprite, float subimage /* = 0.0f*/,
    float imageSpeed /* = 1.0f*/
)
    : SpriteStatic(tex, sprite, subimage)
    , m_imageSpeed(imageSpeed) {
}

void SpriteAnimated::step() {
    m_subimageSprite.x += speed();
    if (m_subimageSprite.x >= m_tex->subimagesSpritesCount().x) {
        m_subimageSprite.x = 0.0f;
    } else if (m_subimageSprite.x < 0.0f) {
        m_subimageSprite.x = m_tex->subimagesSpritesCount().x + speed();
    }
}

void SpriteAnimated::setSpeed(float newSpeed) {
    m_imageSpeed = newSpeed;
}

float SpriteAnimated::speed() const {
    return m_imageSpeed;
}

SpriteComplex::SpriteComplex(
    const TextureShaped& tex, float sprite, float subimage, float imageSpeed,
    Color color, glm::vec2 scale
)
    : SpriteAnimated(tex, sprite, subimage, imageSpeed)
    , m_color(color)
    , m_scale(scale) {
}

void SpriteComplex::setColor(Color color) {
    m_color = color;
}

void SpriteComplex::setScale(glm::vec2 scale) {
    m_scale = scale;
}

Color SpriteComplex::color() const {
    return m_color;
}

glm::vec2 SpriteComplex::scale() const {
    return m_scale;
}

} // namespace re
