/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vector>

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
 * @brief Draws sprites, surfaces and other textures
*/
class SpriteBatch {
public:

    /**
     * @brief Constructs SpriteBatch
    */
    SpriteBatch(int maxSprites);

    void begin();
    void end();

    /**
     * @brief Draws the batch
    */
    void draw(const vk::CommandBuffer& commandBuffer, const glm::mat4& mvpMat);

    //UNCOLORED
    void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, const Texture& tex, int depth);//Rotated to the right
    void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, TextureProxy tex, int depth, float radAngle, const glm::vec2& origin = glm::vec2(0.0f, 0.0f));//Rotated based on the angle
    void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, TextureProxy tex, int depth, const glm::vec2& direction, const glm::vec2& origin = glm::vec2(0.0f, 0.0f));//Rotated based on the vector
    //COLORED
    void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, TextureProxy tex, int depth, Color color);//Rotated to the right
    void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, TextureProxy tex, int depth, Color color, float radAngle, const glm::vec2& origin = glm::vec2(0.0f, 0.0f));//Rotated based on the angle
    void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, TextureProxy tex, int depth, Color color, const glm::vec2& direction, const glm::vec2& origin = glm::vec2(0.0f, 0.0f));//Rotated based on the vector

    //UNCOLORED, UNSTRETCHED
    void addTexture(const Texture& tex, const glm::vec2& position, int depth);//Unrotated
    void addTexture(const Texture& tex, const glm::vec2& position, int depth, float radAngle);//Rotated based on the angle
    void addTexture(const Texture& tex, const glm::vec2& position, int depth, const glm::vec2& direction);//Rotated based on the vector
    //COLORED, STRETCHED
    void addTexture(const Texture& tex, const glm::vec2& position, int depth, Color color, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Unrotated
    void addTexture(const Texture& tex, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Rotated based on the angle
    void addTexture(const Texture& tex, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale);//Rotated based on the vector

    //UNCOLORED, UNSTRETCHED
    void addSprite(const SpriteStatic& sprite, const glm::vec2& position, int depth);//Unrotated
    void addSprite(const SpriteStatic& sprite, const glm::vec2& position, int depth, float radAngle);//Rotated based on the angle
    void addSprite(const SpriteStatic& sprite, const glm::vec2& position, int depth, const glm::vec2& direction);//Rotated based on the vector
    //COLORED, STRETCHED
    void addSprite(const SpriteStatic& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Unrotated
    void addSprite(const SpriteStatic& sprite, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Rotated based on the angle
    void addSprite(const SpriteStatic& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale);//Rotated based on the vector

    //COLORED, STRETCHED BY FULLSPRITE
    void addSprite(const SpriteComplex& sprite, const glm::vec2& position, int depth);//Unrotated
    void addSprite(const SpriteComplex& sprite, const glm::vec2& position, int depth, float radAngle);//Rotated based on the angle
    void addSprite(const SpriteComplex& sprite, const glm::vec2& position, int depth, const glm::vec2& direction);//Rotated based on the vector
    //COLORED, STRETCHED BY USER
    void addSprite(const SpriteComplex& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Unrotated
    void addSprite(const SpriteComplex& sprite, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Rotated based on the angle
    void addSprite(const SpriteComplex& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale);//Rotated based on the vector

    //UNCOLORED, UNSTRETCHED
    void addSubimage(const Texture& tex, const glm::vec2& position, int depth, const glm::vec2& subImg_Spr);//Unrotated
    void addSubimage(const Texture& tex, const glm::vec2& position, int depth, float radAngle, const glm::vec2& subImg_Spr);//Rotated based on the angle
    void addSubimage(const Texture& tex, const glm::vec2& position, int depth, const glm::vec2& direction, const glm::vec2& subImg_Spr);//Rotated based on the vector
    //COLORED, STRETCHED
    void addSubimage(const Texture& tex, const glm::vec2& position, int depth, Color color, const glm::vec2& subImg_Spr, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Unrotated
    void addSubimage(const Texture& tex, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& subImg_Spr, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Rotated based on the angle
    void addSubimage(const Texture& tex, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& subImg_Spr, const glm::vec2& scale);//Rotated based on the vector

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
    int m_maxSprites;
    int m_spriteCount = 0;

    Pipeline m_pipeline;
    DescriptorSet m_descSet{m_pipeline};
    vk::PipelineVertexInputStateCreateInfo createVertexInputStateInfo() const;

    static inline constexpr glm::vec4 UV_RECT = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    static inline constexpr glm::vec4 SUV_RECT = glm::vec4(0.0f, 1.0f, 1.0f, -1.0f);//Used for drawing surfaces
    static inline constexpr Color WHITE{255, 255, 255, 255};
};

}
