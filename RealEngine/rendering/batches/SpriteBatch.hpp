﻿/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <RealEngine/rendering/pipelines/Pipeline.hpp>
#include <RealEngine/rendering/buffers/Buffer.hpp>
#include <RealEngine/rendering/textures/Texture.hpp>
#include <RealEngine/rendering/batches/Sprite.hpp>
#include <RealEngine/rendering/DescriptorSet.hpp>

namespace RE {

enum class GlyphSortType {
    NONE,
    NEG_TOP,
    POS_TOP,
    TEXTURE
};

class Glyph {
public:
    Glyph(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy tex, int depth, Color color);
    Glyph(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy tex, int depth, Color color, float radAngle, const glm::vec2& origin = glm::vec2(0.0f, 0.0f));

    TextureProxy tex;
    int depth;

    VertexPOCOUV topLeft;
    VertexPOCOUV topRight;
    VertexPOCOUV botLeft;
    VertexPOCOUV botRight;
};

struct DrawBatch {
    DrawBatch(int offset, unsigned int count, TextureProxy tex) : offset(offset), count(count), tex(tex) {};

    int offset;
    int count;
    TextureProxy tex;
};

/**
 * @brief Draws sprites, surfaces and other textures
*/
class SpriteBatch {
public:

    /**
     * @brief Constructs new SpriteBatch that will draw with given shaders
     * @details The pipeline created from the shaders is stored inside the sprite batch.
    */
    SpriteBatch(const PipelineSources& sources);

    void begin();
    void end(GlyphSortType sortType);

    //UNCOLORED
    void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, TextureProxy tex, int depth);//Rotated to the right
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

    /**
     * @brief Draws the batch with the stored pipeline
    */
    void draw(const vk::ArrayProxyNoTemporaries<DescriptorSet>& descriptorSets);

    /**
     * @brief Draws once with different pipeline  (the pipeline is not stored)
    */
    void draw(const vk::ArrayProxyNoTemporaries<DescriptorSet>& descriptorSets, const Pipeline& pipeline);

    /**
     * @brief Changes to a different pipeline that will be used for drawing
    */
    void changePipeline(const PipelineSources& sources);

    const Pipeline& getPipeline() const { return m_pipeline; }

private:

    void sortGlyphs(GlyphSortType sortType);
    void createDrawBatches();

    using enum vk::BufferUsageFlagBits;
    using enum vk::MemoryPropertyFlagBits;
    Buffer m_vbo{sizeof(VertexPOCOUV) * 512, eVertexBuffer, eHostVisible | eHostCoherent};

    std::vector<Glyph*> m_glyphPointers;
    std::vector<Glyph> m_glyphs;
    std::vector<DrawBatch> m_drawBatches;
    VertexPOCOUV* m_vertices = nullptr;

    static constexpr bool compareNegToPos(Glyph* a, Glyph* b) { return (a->depth > b->depth); }
    static constexpr bool comparePosToNeg(Glyph* a, Glyph* b) { return (a->depth < b->depth); }
    static constexpr bool compareTexture(Glyph* a, Glyph* b) { return (a->tex > b->tex); }

    Pipeline m_pipeline;
    vk::PipelineVertexInputStateCreateInfo createVertexInputStateInfo() const;

    static inline constexpr glm::vec4 UV_RECT = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    static inline constexpr glm::vec4 SUV_RECT = glm::vec4(0.0f, 1.0f, 1.0f, -1.0f);//Used for drawing surfaces
    static inline constexpr Color WHITE{255, 255, 255, 255};
};

}
