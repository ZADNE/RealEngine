/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/batches/Sprite.hpp>

namespace RE {

SpriteStatic::SpriteStatic(const Texture& tex, float sprite, float subimage/* = 0.0f*/) :
    m_tex(&tex), m_subimageSprite(subimage, sprite) {
}

float SpriteStatic::getSpeed() const {
    return 1.0f;
}

Color SpriteStatic::getColor() const {
    return Color{255u, 255u, 255u, 255u};
}

glm::vec2 SpriteStatic::getScale() const {
    return glm::vec2(1.0f, 1.0f);
}

glm::vec2 SpriteStatic::getSubimageSprite() const {
    return m_subimageSprite;
}

void SpriteStatic::step() {

}

SpriteAnimated::SpriteAnimated(const Texture& tex, float sprite, float subimage/* = 0.0f*/, float imageSpeed/* = 1.0f*/) :
    SpriteStatic(tex, sprite, subimage), m_imageSpeed(imageSpeed) {
}

void SpriteAnimated::step() {
    this->m_subimageSprite.x += getSpeed();
    if ((this->m_subimageSprite.x) >= this->m_tex->getSubimagesSpritesCount().x) {//Positive image speed
        this->m_subimageSprite.x = 0.0f;
    } else if (this->m_subimageSprite.x < 0.0f) {//Negative image speed
        this->m_subimageSprite.x = this->m_tex->getSubimagesSpritesCount().x + getSpeed();
    }
}

void SpriteAnimated::setSpeed(float newSpeed) {
    m_imageSpeed = newSpeed;
}

float SpriteAnimated::getSpeed() const {
    return m_imageSpeed;
}

SpriteComplex::SpriteComplex(const Texture& tex, float sprite, float subimage, float imageSpeed, Color color, const glm::vec2& scale) :
    SpriteAnimated(tex, sprite, subimage, imageSpeed), m_color(color), m_scale(scale) {
}

void SpriteComplex::setColor(Color color) {
    m_color = color;
}

void SpriteComplex::setScale(const glm::vec2& scale) {
    m_scale = scale;
}

Color SpriteComplex::getColor() const {
    return m_color;
}

glm::vec2 SpriteComplex::getScale() const {
    return m_scale;
}

}