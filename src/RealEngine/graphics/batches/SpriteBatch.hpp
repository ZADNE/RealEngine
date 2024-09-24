/**
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

struct SpriteBatchCreateInfo {
    /**
     * @brief The renderpass that the batch will always draw in
     */
    RenderPassSubpass renderPassSubpass{};
    /**
     * @brief Maximum number of sprites that can be in the batch
     */
    unsigned int maxSprites = 0u;
    /**
     * @brief Maximum number of unique textures that the sprites
     */
    unsigned int maxTextures = 0u;
};

/**
 * @brief   Draws 2D sprites efficiently
 * @details Can draw multiple batches per frame.
 *          Each batch has its own transformation matrix
 */
class SpriteBatch {
public:
    /**
     * @brief Constructs a Spritebatch
     */
    explicit SpriteBatch(const SpriteBatchCreateInfo& createInfo);

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
     * @details Sprites of the batch are drawn in the order they were added in
     * @param cb Command buffer used for rendering
     * @param mvpMat Transformation matrix applied to the batch
     */
    void drawBatch(const CommandBuffer& cb, const glm::mat4& mvpMat);

    constexpr static Color k_white{255, 255, 255, 255};

    void add(
        const Texture& tex, const glm::vec4& posSizeRect,
        const glm::vec4& uvsSizeRect, Color col = k_white
    );

    void addSprite(const SpriteStatic& sprite, glm::vec2 pos, Color col = k_white);

    void addSprite(const SpriteComplex& sprite, glm::vec2 pos, Color col = k_white);

    void addSubimage(
        const TextureShaped& tex, glm::vec2 pos, glm::vec2 subimgSpr, Color col = k_white
    );

    const Pipeline& pipeline() const { return m_pipeline; }

private:
    using enum vk::BufferUsageFlagBits;
    using enum vk::MemoryPropertyFlagBits;

    struct alignas(16) Sprite {
        glm::vec4 pos;
        glm::vec4 uvs;
        glm::uint tex;
        Color col;
    };
    BufferMapped<Sprite> m_spritesBuf;
    std::vector<const Texture*> m_texToIndex;
    unsigned int m_maxSprites;
    unsigned int m_maxTextures;
    unsigned int m_nextSpriteIndex       = 0;
    unsigned int m_batchFirstSpriteIndex = 0;
    unsigned int m_textureIndexOffset    = 0;

    unsigned int nextSpriteIndex();
    unsigned int texToIndex(const Texture& tex);

    PipelineLayout m_pipelineLayout;
    static PipelineLayout createPipelineLayout(unsigned int maxTextures);
    Pipeline m_pipeline;
    static Pipeline createPipeline(
        const PipelineLayout& pipelineLayout, const SpriteBatchCreateInfo& createInfo
    );
    DescriptorSet m_descSet{{.layout = m_pipelineLayout.descriptorSetLayout(0)}};
};

} // namespace re
