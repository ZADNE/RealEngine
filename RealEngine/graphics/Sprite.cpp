/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/Sprite.hpp>

#include <RealEngine/graphics/textures/Texture.hpp>

namespace RE {

	Sprite::Sprite(TexturePtr texture, float sprite, float subimage/* = 0.0f*/) :
		p_texture(texture),
		p_subimageSprite(subimage, sprite) {

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
		return p_subimageSprite;
	}

	void Sprite::step() {

	}

	SpeedSprite::SpeedSprite(TexturePtr texture, float sprite, float subimage/* = 0.0f*/, float imageSpeed/* = 1.0f*/) :
		Sprite(texture, sprite, subimage),
		p_imageSpeed(imageSpeed) {

	}

	void SpeedSprite::step() {
		p_subimageSprite.x += getSpeed();
		if ((p_subimageSprite.x) >= p_texture->getSubimagesSpritesCount().x) {//Positive image speed
			p_subimageSprite.x = 0.0f;
		}
		else if (p_subimageSprite.x < 0.0f) {//Negative image speed
			p_subimageSprite.x = p_texture->getSubimagesSpritesCount().x + getSpeed();
		}
	}

	void SpeedSprite::setSpeed(float newSpeed) {
		p_imageSpeed = newSpeed;
	}

	float SpeedSprite::getSpeed() const {
		return p_imageSpeed;
	}

	FullSprite::FullSprite(TexturePtr texture, float sprite, float subimage, float imageSpeed, RE::Color color, const glm::vec2& scale) :
		SpeedSprite{ texture, sprite, subimage, imageSpeed },
		p_color(color),
		p_scale(scale) {

	}

	void FullSprite::setColor(RE::Color color) {
		p_color = color;
	}

	void FullSprite::setScale(const glm::vec2& scale) {
		p_scale = scale;
	}

	RE::Color FullSprite::getColor() const {
		return p_color;
	}

	glm::vec2 FullSprite::getScale() const {
		return p_scale;
	}

}