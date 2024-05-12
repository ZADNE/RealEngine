/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <glm/vec2.hpp>

#include <RealEngine/graphics/pipelines/Vertex.hpp>
#include <RealEngine/graphics/textures/TextureShaped.hpp>

namespace re {

/**
 * @brief Represents a static subimage within a texture
 */
class SpriteStatic {
public:
    /**
     * @warning Sprite does not hold ownership of the Texture!
     */
    SpriteStatic(const TextureShaped& tex, float sprite, float subimage = 0.0f);

    virtual float speed() const;
    virtual Color color() const;
    virtual glm::vec2 scale() const;
    virtual glm::vec2 subimageSprite() const;

    virtual void step();

    const TextureShaped& texture() const { return *m_tex; };

protected:
    glm::vec2 m_subimageSprite; /**< X = subimage of the sprite, Y = sprite of the texture */
    const TextureShaped* m_tex;
};

/**
 * @brief Represents an animated subimage within a texture
 */
class SpriteAnimated: public SpriteStatic {
public:
    SpriteAnimated(
        const TextureShaped& tex, float sprite, float subimage = 0.0f,
        float imageSpeed = 1.0f
    );

    void step() override;

    void setSpeed(float newSpeed);

    float speed() const override;

protected:
    float m_imageSpeed; /**< Is added each step to the current image position */
};

/**
 * @brief Represents an animated subimage within a texture that is tinted and scaled
 */
class SpriteComplex: public SpriteAnimated {
public:
    SpriteComplex(
        const TextureShaped& tex, float sprite, float subimage,
        float imageSpeed, Color color, glm::vec2 scale
    );

    void setColor(Color color);
    void setScale(glm::vec2 scale);

    Color color() const override;
    glm::vec2 scale() const override;

protected:
    glm::vec2 m_scale;
    Color m_color;
};

} // namespace re
