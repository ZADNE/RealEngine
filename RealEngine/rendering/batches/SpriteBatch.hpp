/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <unordered_map>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <RealEngine/rendering/pipelines/Pipeline.hpp>
#include <RealEngine/rendering/buffers/Buffer.hpp>
#include <RealEngine/rendering/textures/Texture.hpp>
#include <RealEngine/rendering/batches/Sprite.hpp>
#include <RealEngine/rendering/DescriptorSet.hpp>

namespace RE {

/**
 * @brief Draws 2D sprites efficiently
*/
class SpriteBatch {
public:

    /**
     * @brief Construct Spritebatch
     * @param maxSprites Maximum number of sprites that can be in the batch
     * @param maxTextures Maximum number of unique textures that the sprites can refer to
    */
    SpriteBatch(unsigned int maxSprites, unsigned int maxTextures);

    /**
     * @brief Begins new batch
     * @detail All sprites have to be added between begin() and end()
    */
    void begin();

    /**
     * @brief Ends the batch
     * @detail All sprites have to be added between begin() and end()
    */
    void end();

    /**
     * @brief Draws the batch
     * @detail All sprites are drawn in the order they were added in
     * @param commandBuffer Command buffer used for rendering
     * @param mvpMat Transformation matrix applied to the sprites
    */
    void draw(const vk::CommandBuffer& commandBuffer, const glm::mat4& mvpMat);

    //UNCOLORED
    void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, const Texture& tex);//Rotated to the right
    void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, TextureProxy tex, float radAngle, const glm::vec2& origin = glm::vec2(0.0f, 0.0f));//Rotated based on the angle
    void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, TextureProxy tex, const glm::vec2& direction, const glm::vec2& origin = glm::vec2(0.0f, 0.0f));//Rotated based on the vector
    //COLORED
    void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, TextureProxy tex, Color color);//Rotated to the right
    void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, TextureProxy tex, Color color, float radAngle, const glm::vec2& origin = glm::vec2(0.0f, 0.0f));//Rotated based on the angle
    void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, TextureProxy tex, Color color, const glm::vec2& direction, const glm::vec2& origin = glm::vec2(0.0f, 0.0f));//Rotated based on the vector

    //UNCOLORED, UNSTRETCHED
    void addTexture(const Texture& tex, const glm::vec2& position);//Unrotated
    void addTexture(const Texture& tex, const glm::vec2& position, float radAngle);//Rotated based on the angle
    void addTexture(const Texture& tex, const glm::vec2& position, const glm::vec2& direction);//Rotated based on the vector
    //COLORED, STRETCHED
    void addTexture(const Texture& tex, const glm::vec2& position, Color color, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Unrotated
    void addTexture(const Texture& tex, const glm::vec2& position, Color color, float radAngle, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Rotated based on the angle
    void addTexture(const Texture& tex, const glm::vec2& position, Color color, const glm::vec2& direction, const glm::vec2& scale);//Rotated based on the vector

    //UNCOLORED, UNSTRETCHED
    void addSprite(const SpriteStatic& sprite, const glm::vec2& position);//Unrotated
    void addSprite(const SpriteStatic& sprite, const glm::vec2& position, float radAngle);//Rotated based on the angle
    void addSprite(const SpriteStatic& sprite, const glm::vec2& position, const glm::vec2& direction);//Rotated based on the vector
    //COLORED, STRETCHED
    void addSprite(const SpriteStatic& sprite, const glm::vec2& position, Color color, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Unrotated
    void addSprite(const SpriteStatic& sprite, const glm::vec2& position, Color color, float radAngle, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Rotated based on the angle
    void addSprite(const SpriteStatic& sprite, const glm::vec2& position, Color color, const glm::vec2& direction, const glm::vec2& scale);//Rotated based on the vector

    //COLORED, STRETCHED BY FULLSPRITE
    void addSprite(const SpriteComplex& sprite, const glm::vec2& position);//Unrotated
    void addSprite(const SpriteComplex& sprite, const glm::vec2& position, float radAngle);//Rotated based on the angle
    void addSprite(const SpriteComplex& sprite, const glm::vec2& position, const glm::vec2& direction);//Rotated based on the vector
    //COLORED, STRETCHED BY USER
    void addSprite(const SpriteComplex& sprite, const glm::vec2& position, Color color, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Unrotated
    void addSprite(const SpriteComplex& sprite, const glm::vec2& position, Color color, float radAngle, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Rotated based on the angle
    void addSprite(const SpriteComplex& sprite, const glm::vec2& position, Color color, const glm::vec2& direction, const glm::vec2& scale);//Rotated based on the vector

    //UNCOLORED, UNSTRETCHED
    void addSubimage(const Texture& tex, const glm::vec2& position, const glm::vec2& subImg_Spr);//Unrotated
    void addSubimage(const Texture& tex, const glm::vec2& position, float radAngle, const glm::vec2& subImg_Spr);//Rotated based on the angle
    void addSubimage(const Texture& tex, const glm::vec2& position, const glm::vec2& direction, const glm::vec2& subImg_Spr);//Rotated based on the vector
    //COLORED, STRETCHED
    void addSubimage(const Texture& tex, const glm::vec2& position, Color color, const glm::vec2& subImg_Spr, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Unrotated
    void addSubimage(const Texture& tex, const glm::vec2& position, Color color, float radAngle, const glm::vec2& subImg_Spr, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Rotated based on the angle
    void addSubimage(const Texture& tex, const glm::vec2& position, Color color, const glm::vec2& direction, const glm::vec2& subImg_Spr, const glm::vec2& scale);//Rotated based on the vector

    const Pipeline& pipeline() const { return m_pipeline; }

private:

    using enum vk::BufferUsageFlagBits;
    using enum vk::MemoryPropertyFlagBits;

    struct alignas(16) Sprite {
        glm::vec4 pos;
        glm::vec4 uvs;
        glm::uint tex;
        RE::Color col;
    };
    Buffer m_spritesBuf;
    Sprite* m_spritesMapped = nullptr;
    std::unordered_map<const Texture*, unsigned int> m_texToIndex;
    unsigned int m_maxSprites;
    unsigned int m_maxTextures;
    unsigned int m_nextSpriteIndex = 0;
    unsigned int m_nextTextureIndex = 0;

    unsigned int texToIndex(const Texture& tex);

    Pipeline m_pipeline;
    DescriptorSet m_descSet{m_pipeline};
    Pipeline createPipeline(unsigned int maxTextures) const;

    static inline constexpr glm::vec4 UV_RECT = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    static inline constexpr glm::vec4 SUV_RECT = glm::vec4(0.0f, 1.0f, 1.0f, -1.0f);//Used for drawing surfaces
    static inline constexpr Color WHITE{255, 255, 255, 255};
};

}
