/*!
 *  @author    Dubsky Tomas
 */
#include <algorithm>

#include <glm/common.hpp>

#include <RealEngine/graphics/batches/SpriteBatch.hpp>
#include <RealEngine/graphics/batches/shaders/AllShaders.hpp>
#include <RealEngine/graphics/synchronization/DoubleBuffered.hpp>

using enum vk::DescriptorBindingFlagBits;

namespace re {

SpriteBatch::SpriteBatch(unsigned int maxSprites, unsigned int maxTextures)
    : m_spritesBuf(BufferCreateInfo{
          .allocFlags = vma::AllocationCreateFlagBits::eMapped |
                        vma::AllocationCreateFlagBits::eHostAccessRandom,
          .sizeInBytes = k_maxFramesInFlight * maxSprites * sizeof(Sprite),
          .usage       = eVertexBuffer})
    , m_maxSprites(maxSprites)
    , m_maxTextures(maxTextures)
    , m_pipelineLayout(createPipelineLayout(maxTextures))
    , m_pipeline(createPipeline(m_pipelineLayout, maxTextures)) {
    m_texToIndex.reserve(maxTextures);
}

void SpriteBatch::clearAndBeginFirstBatch() {
    m_nextSpriteIndex = m_maxSprites * FrameDoubleBufferingState::writeIndex();
    m_textureIndexOffset = m_maxTextures * FrameDoubleBufferingState::writeIndex();
    m_texToIndex.clear();
    nextBatch();
}

void SpriteBatch::nextBatch() {
    m_batchFirstSpriteIndex = m_nextSpriteIndex;
}

void SpriteBatch::drawBatch(
    const vk::CommandBuffer& commandBuffer, const glm::mat4& mvpMat
) {
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *m_pipeline);
    commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eGraphics, *m_pipelineLayout, 0u, *m_descSet, {}
    );
    commandBuffer.bindVertexBuffers(0u, m_spritesBuf.buffer(), vk::DeviceSize{0});
    commandBuffer.pushConstants<glm::mat4>(
        *m_pipelineLayout, vk::ShaderStageFlagBits::eTessellationEvaluation, 0u, mvpMat
    );
    commandBuffer.draw(
        m_nextSpriteIndex - m_batchFirstSpriteIndex, 1u, m_batchFirstSpriteIndex, 0u
    );
}

void SpriteBatch::add(
    const Texture& tex, const glm::vec4& posSizeRect, const glm::vec4& uvsSizeRect
) {
    m_spritesBuf[m_nextSpriteIndex++] = Sprite{
        .pos = posSizeRect, .uvs = uvsSizeRect, .tex = texToIndex(tex), .col = k_white};
}

void SpriteBatch::addSprite(const SpriteStatic& sprite, const glm::vec2& pos) {
    const auto& tex                   = sprite.texture();
    m_spritesBuf[m_nextSpriteIndex++] = Sprite{
        .pos = glm::vec4(pos - tex.pivot(), tex.subimageDims()),
        .uvs = glm::vec4(
            glm::floor(sprite.subimageSprite()) / tex.subimagesSpritesCount(),
            glm::vec2(1.0f, 1.0f) / tex.subimagesSpritesCount()
        ),
        .tex = texToIndex(tex),
        .col = k_white};
}

void SpriteBatch::addSprite(const SpriteComplex& sprite, const glm::vec2& pos) {
    const auto& tex                   = sprite.texture();
    m_spritesBuf[m_nextSpriteIndex++] = Sprite{
        .pos = glm::vec4(
            pos - tex.pivot() * sprite.scale(), tex.subimageDims() * sprite.scale()
        ),
        .uvs = glm::vec4(
            glm::floor(sprite.subimageSprite()) / tex.subimagesSpritesCount(),
            glm::vec2(1.0f, 1.0f) / tex.subimagesSpritesCount()
        ),
        .tex = texToIndex(tex),
        .col = k_white};
}

void SpriteBatch::addSubimage(
    const TextureShaped& tex, const glm::vec2& pos, const glm::vec2& subimgSpr
) {
    m_spritesBuf[m_nextSpriteIndex++] = Sprite{
        .pos = glm::vec4(pos - tex.pivot(), tex.subimageDims()),
        .uvs = glm::vec4(
            glm::floor(subimgSpr) / tex.subimagesSpritesCount(),
            glm::vec2(1.0f, 1.0f) / tex.subimagesSpritesCount()
        ),
        .tex = texToIndex(tex),
        .col = k_white};
}

unsigned int SpriteBatch::texToIndex(const Texture& tex) {
    if (auto it = std::find(m_texToIndex.begin(), m_texToIndex.end(), &tex);
        it != m_texToIndex.end()) {
        return m_textureIndexOffset + (it - m_texToIndex.begin());
    } else {
        unsigned int newIndex = m_texToIndex.size() + m_textureIndexOffset;
        m_descSet.write(
            vk::DescriptorType::eCombinedImageSampler,
            0u,
            newIndex,
            tex,
            vk::ImageLayout::eReadOnlyOptimal
        );
        m_texToIndex.emplace_back(&tex);
        return newIndex;
    }
}

PipelineLayout SpriteBatch::createPipelineLayout(unsigned int maxTextures) {
    // Specialization constants
    static constexpr vk::SpecializationMapEntry specMapEntry{0u, 0u, 4ull};
    unsigned int totalTextures = maxTextures * k_maxFramesInFlight;
    static constexpr vk::DescriptorBindingFlags bindingFlags = {
        eUpdateUnusedWhilePending | ePartiallyBound};
    auto bindingFlagsArray = vk::ArrayProxy<vk::DescriptorBindingFlags>(bindingFlags
    );
    return PipelineLayout{
        PipelineLayoutCreateInfo{
            .descriptorBindingFlags = {bindingFlagsArray},
            .specializationInfo =
                vk::SpecializationInfo{
                    1u, &specMapEntry, sizeof(totalTextures), &totalTextures}},
        PipelineGraphicsSources{
            .vert = sprite_vert,
            .tesc = sprite_tesc,
            .tese = sprite_tese,
            .frag = sprite_frag}};
}

Pipeline SpriteBatch::createPipeline(
    const PipelineLayout& pipelineLayout, unsigned int maxTextures
) {
    // Vertex input
    static constexpr std::array bindings =
        std::to_array<vk::VertexInputBindingDescription>({{
            0u,                          // Binding index
            sizeof(Sprite),              // Stride
            vk::VertexInputRate::eVertex // Input rate
        }});
    static constexpr std::array attributes =
        std::to_array<vk::VertexInputAttributeDescription>(
            {{
                 0u,                              // Location
                 0u,                              // Binding index
                 vk::Format::eR32G32B32A32Sfloat, // Format
                 offsetof(Sprite, pos)            // Relative offset
             },
             {
                 1u,                              // Location
                 0u,                              // Binding index
                 vk::Format::eR32G32B32A32Sfloat, // Format
                 offsetof(Sprite, uvs)            // Relative offset
             },
             {
                 2u,                   // Location
                 0u,                   // Binding index
                 vk::Format::eR32Uint, // Format
                 offsetof(Sprite, tex) // Relative offset
             },
             {
                 3u,                   // Location
                 0u,                   // Binding index
                 vk::Format::eR32Uint, // Format
                 offsetof(Sprite, col) // Relative offset
             }}
        );
    // Specialization constants
    static constexpr vk::SpecializationMapEntry k_specMapEntry{0u, 0u, 4ull};
    unsigned int totalTextures = maxTextures * k_maxFramesInFlight;
    return Pipeline{
        PipelineGraphicsCreateInfo{
            .pipelineLayout = *pipelineLayout,
            .vertexInput =
                vk::PipelineVertexInputStateCreateInfo{{}, bindings, attributes},
            .topology           = vk::PrimitiveTopology::ePatchList,
            .patchControlPoints = 1u,
            .specializationInfo =
                vk::SpecializationInfo{
                    1u, &k_specMapEntry, sizeof(totalTextures), &totalTextures}},
        PipelineGraphicsSources{
            .vert = sprite_vert,
            .tesc = sprite_tesc,
            .tese = sprite_tese,
            .frag = sprite_frag}};
}

} // namespace re
