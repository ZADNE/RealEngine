/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/batches/Sprite.hpp>

#include <RealEngine/rendering/textures/Texture.hpp>

namespace RE {

template<Renderer R>
SpriteStatic<R>::SpriteStatic(const Texture<R>& tex, float sprite, float subimage/* = 0.0f*/) :
    m_tex(tex), m_subimageSprite(subimage, sprite) {
}

template<Renderer R>
float SpriteStatic<R>::getSpeed() const {
    return 1.0f;
}

template<Renderer R>
Color SpriteStatic<R>::getColor() const {
    return Color{ 255u, 255u, 255u, 255u };
}

template<Renderer R>
glm::vec2 SpriteStatic<R>::getScale() const {
    return glm::vec2(1.0f, 1.0f);
}

template<Renderer R>
glm::vec2 SpriteStatic<R>::getSubimageSprite() const {
    return m_subimageSprite;
}

template<Renderer R>
void SpriteStatic<R>::step() {

}

template<Renderer R>
SpriteAnimated<R>::SpriteAnimated(const Texture<R>& tex, float sprite, float subimage/* = 0.0f*/, float imageSpeed/* = 1.0f*/) :
    SpriteStatic<R>(tex, sprite, subimage), m_imageSpeed(imageSpeed) {
}

template<Renderer R>
void SpriteAnimated<R>::step() {
    this->m_subimageSprite.x += getSpeed();
    if ((this->m_subimageSprite.x) >= this->m_tex.getSubimagesSpritesCount().x) {//Positive image speed
        this->m_subimageSprite.x = 0.0f;
    } else if (this->m_subimageSprite.x < 0.0f) {//Negative image speed
        this->m_subimageSprite.x = this->m_tex.getSubimagesSpritesCount().x + getSpeed();
    }
}

template<Renderer R>
void SpriteAnimated<R>::setSpeed(float newSpeed) {
    m_imageSpeed = newSpeed;
}

template<Renderer R>
float SpriteAnimated<R>::getSpeed() const {
    return m_imageSpeed;
}

template<Renderer R>
SpriteComplex<R>::SpriteComplex(const Texture<R>& tex, float sprite, float subimage, float imageSpeed, Color color, const glm::vec2& scale) :
    SpriteAnimated<R>(tex, sprite, subimage, imageSpeed), m_color(color), m_scale(scale) {
}

template<Renderer R>
void SpriteComplex<R>::setColor(Color color) {
    m_color = color;
}

template<Renderer R>
void SpriteComplex<R>::setScale(const glm::vec2& scale) {
    m_scale = scale;
}

template<Renderer R>
Color SpriteComplex<R>::getColor() const {
    return m_color;
}

template<Renderer R>
glm::vec2 SpriteComplex<R>::getScale() const {
    return m_scale;
}

template SpriteStatic<RendererLateBind>;
template SpriteStatic<RendererGL46>;
template SpriteAnimated<RendererLateBind>;
template SpriteAnimated<RendererGL46>;
template SpriteComplex<RendererLateBind>;
template SpriteComplex<RendererGL46>;

}