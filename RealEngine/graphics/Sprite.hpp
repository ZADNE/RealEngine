#pragma once
#include <glm/vec2.hpp>

#include <RealEngine/graphics/Vertex.hpp>
#include <RealEngine/resources/ResourceManager.hpp>

namespace RE {

class Texture;

//SPRITE (static)
class Sprite {
public:
	Sprite(TexturePtr texture, float sprite, float subimage = 0.0f);

	virtual float getSpeed() const;
	virtual RE::Colour getColour() const;
	virtual glm::vec2 getScale() const;
	virtual glm::vec2 getSubimageSprite() const;

	virtual void step();

	const Texture* getTexture() const { return p_texture.get(); };
protected:
	glm::vec2 p_subimageSprite;//X = subimage of the sprite, Y = sprite of the texture
	TexturePtr p_texture;
};

//SPEED SPRITE (images animate, user can specify image speed)
class SpeedSprite : public Sprite {
public:
	SpeedSprite(TexturePtr texture, float sprite, float subimage = 0.0f, float imageSpeed = 1.0f);

	void step() override;

	void setSpeed(float newSpeed);

	float getSpeed() const override;
protected:
	float p_imageSpeed;//Speed is added each beginStep to the current image position
};

//FULL SPRITE (user can specify default colour and draw scale)
class FullSprite : public SpeedSprite {
public:
	FullSprite(TexturePtr texture, float sprite, float subimage, float imageSpeed, RE::Colour colour, const glm::vec2& scale);

	void setColour(RE::Colour colour);
	void setScale(const glm::vec2& scale);

	virtual RE::Colour getColour() const override;
	virtual glm::vec2 getScale() const override;
protected:
	glm::vec2 p_scale;
	RE::Colour p_colour;
};

}
