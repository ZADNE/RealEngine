/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <RealEngine/graphics/batches/Sprite.hpp>
#include <RealEngine/graphics/buffers/BufferMapped.hpp>
#include <RealEngine/graphics/descriptors/DescriptorSet.hpp>
#include <RealEngine/graphics/pipelines/Pipeline.hpp>
#include <RealEngine/graphics/pipelines/PipelineLayout.hpp>

namespace re {

/**
 * @brief   Draws 2D sprites efficiently
 * @details Can draw multiple batches per frame.
 *          Each batch has its own transformation matrix
 */
class SpriteBatch {
public:
    /**
     * @brief Construct Spritebatch
     * @param maxSprites Maximum number of sprites that can be in the batch
     * @param maxTextures Maximum number of unique textures that the sprites
     * can refer to
     */
    SpriteBatch(unsigned int maxSprites, unsigned int maxTextures);

    /**
     * @brief   Resets the sprite batch, also begins first batch
     * @details Call this at the beginning of each frame.
     */
    void clearAndBeginFirstBatch();

    /**
     * @brief   Begins new batch
     * @details There can be multiple batches in a frame, this function
     *          separates them
     */
    void nextBatch();

    /**
     * @brief   Draws the last batch
     * @details Sprite in the batch are drawn in the order they were added in
     * @param cmdBuf Command buffer used for rendering
     * @param mvpMat Transformation matrix applied to the batch
     */
    void drawBatch(const vk::CommandBuffer& cmdBuf, const glm::mat4& mvpMat);

    void add(const Texture& tex, const glm::vec4& posSizeRect, const glm::vec4& uvsSizeRect);

    void addSprite(const SpriteStatic& sprite, const glm::vec2& pos);

    void addSprite(const SpriteComplex& sprite, const glm::vec2& pos);

    void addSubimage(
        const TextureShaped& tex, const glm::vec2& pos, const glm::vec2& subimgSpr
    );

    const Pipeline& pipeline() const { return m_pipeline; }

private:
    using enum vk::BufferUsageFlagBits;
    using enum vk::MemoryPropertyFlagBits;

    struct alignas(16) Sprite {
        glm::vec4 pos;
        glm::vec4 uvs;
        glm::uint tex;
        Color     col;
    };
    BufferMapped<Sprite>        m_spritesBuf;
    std::vector<const Texture*> m_texToIndex;
    unsigned int                m_maxSprites;
    unsigned int                m_maxTextures;
    unsigned int                m_nextSpriteIndex       = 0;
    unsigned int                m_batchFirstSpriteIndex = 0;
    unsigned int                m_textureIndexOffset    = 0;

    unsigned int texToIndex(const Texture& tex);

    PipelineLayout        m_pipelineLayout;
    static PipelineLayout createPipelineLayout(unsigned int maxTextures);
    Pipeline              m_pipeline;
    static Pipeline       createPipeline(
              const PipelineLayout& pipelineLayout, unsigned int maxTextures
          );
    DescriptorSet m_descSet{m_pipelineLayout.descriptorSetLayout(0)};

    static inline constexpr Color k_white{255, 255, 255, 255};
};

} // namespace re
