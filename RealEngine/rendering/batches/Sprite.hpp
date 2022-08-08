/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <glm/vec2.hpp>

#include <RealEngine/rendering/vertices/vertices.hpp>
#include <RealEngine/resources/ResourceManager.hpp>

namespace RE {

	class Texture;

	/**
	 * @brief Represents a static subimage within a texture.
	*/
	class Sprite {
	public:
		Sprite(TexturePtr texture, float sprite, float subimage = 0.0f);

		virtual float getSpeed() const;
		virtual RE::Color getColor() const;
		virtual glm::vec2 getScale() const;
		virtual glm::vec2 getSubimageSprite() const;

		virtual void step();

		const Texture* getTexture() const { return m_texture.get(); };
	protected:
		glm::vec2 m_subimageSprite;//X = subimage of the sprite, Y = sprite of the texture
		TexturePtr m_texture;
	};

	/**
	 * @brief Represents an animated subimage within a texture.
	*/
	class SpeedSprite : public Sprite {
	public:
		SpeedSprite(TexturePtr texture, float sprite, float subimage = 0.0f, float imageSpeed = 1.0f);

		void step() override;

		void setSpeed(float newSpeed);

		float getSpeed() const override;
	protected:
		float m_imageSpeed;//Speed is added each beginStep to the current image position
	};

	/**
	 * @brief Represents an animated subimage within a texture that can be tinted or scaled.
	*/
	class FullSprite : public SpeedSprite {
	public:
		FullSprite(TexturePtr texture, float sprite, float subimage, float imageSpeed, RE::Color color, const glm::vec2& scale);

		void setColor(RE::Color color);
		void setScale(const glm::vec2& scale);

		virtual RE::Color getColor() const override;
		virtual glm::vec2 getScale() const override;
	protected:
		glm::vec2 m_scale;
		RE::Color m_color;
	};

}
