/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/batches/Sprite.hpp>

#include <RealEngine/rendering/textures/Texture.hpp>

namespace RE {

	Sprite::Sprite(TexturePtr texture, float sprite, float subimage/* = 0.0f*/) :
		m_texture(texture),
		m_subimageSprite(subimage, sprite) {

	}

	float Sprite::getSpeed() const {
		return 1.0f;
	}

	RE::Color Sprite::getColor() const {
		return RE::Color{ 255u, 255u, 255u, 255u };
	}

	glm::vec2 Sprite::getScale() const {
		return glm::vec2(1.0f, 1.0f);
	}

	glm::vec2 Sprite::getSubimageSprite() const {
		return m_subimageSprite;
	}

	void Sprite::step() {

	}

	SpeedSprite::SpeedSprite(TexturePtr texture, float sprite, float subimage/* = 0.0f*/, float imageSpeed/* = 1.0f*/) :
		Sprite(texture, sprite, subimage),
		m_imageSpeed(imageSpeed) {

	}

	void SpeedSprite::step() {
		m_subimageSprite.x += getSpeed();
		if ((m_subimageSprite.x) >= m_texture->getSubimagesSpritesCount().x) {//Positive image speed
			m_subimageSprite.x = 0.0f;
		}
		else if (m_subimageSprite.x < 0.0f) {//Negative image speed
			m_subimageSprite.x = m_texture->getSubimagesSpritesCount().x + getSpeed();
		}
	}

	void SpeedSprite::setSpeed(float newSpeed) {
		m_imageSpeed = newSpeed;
	}

	float SpeedSprite::getSpeed() const {
		return m_imageSpeed;
	}

	FullSprite::FullSprite(TexturePtr texture, float sprite, float subimage, float imageSpeed, RE::Color color, const glm::vec2& scale) :
		SpeedSprite{ texture, sprite, subimage, imageSpeed },
		m_color(color),
		m_scale(scale) {

	}

	void FullSprite::setColor(RE::Color color) {
		m_color = color;
	}

	void FullSprite::setScale(const glm::vec2& scale) {
		m_scale = scale;
	}

	RE::Color FullSprite::getColor() const {
		return m_color;
	}

	glm::vec2 FullSprite::getScale() const {
		return m_scale;
	}

}