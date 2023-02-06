﻿/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/batches/SpriteBatch.hpp>

#include <algorithm>

#include <glm/common.hpp>

#include <RealEngine/rendering/batches/shaders/AllShaders.hpp>
#include <RealEngine/rendering/PerFrameInFlight.hpp>

using enum vk::DescriptorBindingFlagBits;

namespace RE {

SpriteBatch::SpriteBatch(unsigned int maxSprites, unsigned int maxTextures) :
    m_spritesBuf(MAX_FRAMES_IN_FLIGHT* maxSprites * sizeof(Sprite), eVertexBuffer, eHostVisible | eHostCoherent),
    m_spritesMapped(m_spritesBuf.map<Sprite>(0u, MAX_FRAMES_IN_FLIGHT* maxSprites * sizeof(Sprite))),
    m_maxSprites(maxSprites),
    m_maxTextures(maxTextures),
    m_pipelineLayout(createPipelineLayout(maxTextures)),
    m_pipeline(createPipeline(m_pipelineLayout, maxTextures)) {
    m_texToIndex.reserve(maxTextures);
}

void SpriteBatch::begin() {
    m_nextSpriteIndex = m_maxSprites * NEXT_FRAME;
    m_nextTextureIndex = m_maxTextures * NEXT_FRAME;
    m_texToIndex.clear();
}

void SpriteBatch::end() {
    //Nothing to do :-)
}

void SpriteBatch::draw(const vk::CommandBuffer& commandBuffer, const glm::mat4& mvpMat) {
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *m_pipeline);
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *m_pipelineLayout, 0u, *m_descSet, {});
    commandBuffer.bindVertexBuffers(0u, *m_spritesBuf, 0ull);
    commandBuffer.pushConstants<glm::mat4>(*m_pipelineLayout, vk::ShaderStageFlagBits::eTessellationEvaluation, 0u, mvpMat);
    commandBuffer.draw(m_nextSpriteIndex - m_maxSprites * NEXT_FRAME, 1u, m_maxSprites * CURRENT_FRAME, 0u);
}

void SpriteBatch::add(const Texture& tex, const glm::vec4& posSizeRect, const glm::vec4& uvsSizeRect) {
    m_spritesMapped[m_nextSpriteIndex++] = Sprite{
        .pos = posSizeRect,
        .uvs = uvsSizeRect,
        .tex = texToIndex(tex),
        .col = WHITE
    };
}

void SpriteBatch::addSprite(const SpriteStatic& sprite, const glm::vec2& pos) {
    const auto& tex = sprite.texture();
    m_spritesMapped[m_nextSpriteIndex++] = Sprite{
        .pos = glm::vec4(pos - tex.pivot(), tex.subimageDims()),
        .uvs = glm::vec4(glm::floor(sprite.subimageSprite()) / tex.subimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.subimagesSpritesCount()),
        .tex = texToIndex(tex),
        .col = WHITE
    };
}

void SpriteBatch::addSprite(const SpriteComplex& sprite, const glm::vec2& pos) {
    const auto& tex = sprite.texture();
    m_spritesMapped[m_nextSpriteIndex++] = Sprite{
        .pos = glm::vec4(pos - tex.pivot(), tex.subimageDims() * sprite.scale()),
        .uvs = glm::vec4(glm::floor(sprite.subimageSprite()) / tex.subimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.subimagesSpritesCount()),
        .tex = texToIndex(tex),
        .col = WHITE
    };
}

void SpriteBatch::addSubimage(const TextureShaped& tex, const glm::vec2& pos, const glm::vec2& subImg_Spr) {
    m_spritesMapped[m_nextSpriteIndex++] = Sprite{
        .pos = glm::vec4(pos - tex.pivot(), tex.subimageDims()),
        .uvs = glm::vec4(glm::floor(subImg_Spr) / tex.subimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.subimagesSpritesCount()),
        .tex = texToIndex(tex),
        .col = WHITE
    };
}

unsigned int SpriteBatch::texToIndex(const Texture& tex) {
    auto it = m_texToIndex.find(&tex);
    if (it != m_texToIndex.end()) {
        return it->second;
    } else {
        m_texToIndex.emplace(&tex, m_nextTextureIndex);
        m_descSet.write(vk::DescriptorType::eCombinedImageSampler, 0u, m_nextTextureIndex, tex);
        return m_nextTextureIndex++;
    }
}

PipelineLayout SpriteBatch::createPipelineLayout(unsigned int maxTextures) {
    //Specialization constants
    static constexpr vk::SpecializationMapEntry SPEC_MAP_ENTRY{0u, 0u, 4ull};
    unsigned int totalTextures = maxTextures * MAX_FRAMES_IN_FLIGHT;
    static constexpr vk::DescriptorBindingFlags bindingFlags = {eUpdateUnusedWhilePending | ePartiallyBound};
    auto bindingFlagsArray = vk::ArrayProxy<vk::DescriptorBindingFlags>(bindingFlags);
    return PipelineLayout{
        PipelineLayoutCreateInfo{
            .descriptorBindingFlags = {bindingFlagsArray},
            .specializationInfo = vk::SpecializationInfo{1u, &SPEC_MAP_ENTRY, sizeof(totalTextures), &totalTextures}
        }, PipelineGraphicsSources{
            .vert = sprite_vert,
            .tesc = sprite_tesc,
            .tese = sprite_tese,
            .frag = sprite_frag
        }
    };
}

Pipeline SpriteBatch::createPipeline(const PipelineLayout& pipelineLayout, unsigned int maxTextures) {
    //Vertex input
    static constexpr std::array bindings = std::to_array<vk::VertexInputBindingDescription>({{
        0u,                             //Binding index
        sizeof(Sprite),                 //Stride
        vk::VertexInputRate::eVertex    //Input rate
    }});
    static constexpr std::array attributes = std::to_array<vk::VertexInputAttributeDescription>({{
        0u,                             //Location
        0u,                             //Binding index
        vk::Format::eR32G32B32A32Sfloat,//Format
        offsetof(Sprite, pos)           //Relative offset
    },{
        1u,                             //Location
        0u,                             //Binding index
        vk::Format::eR32G32B32A32Sfloat,//Format
        offsetof(Sprite, uvs)           //Relative offset
    },{
        2u,                             //Location
        0u,                             //Binding index
        vk::Format::eR32Uint,           //Format
        offsetof(Sprite, tex)           //Relative offset
    },{
        3u,                             //Location
        0u,                             //Binding index
        vk::Format::eR32Uint,           //Format
        offsetof(Sprite, col)           //Relative offset
    }});
    //Specialization constants
    static constexpr vk::SpecializationMapEntry SPEC_MAP_ENTRY{0u, 0u, 4ull};
    unsigned int totalTextures = maxTextures * MAX_FRAMES_IN_FLIGHT;
    return Pipeline{
        PipelineGraphicsCreateInfo{
            .pipelineLayout = *pipelineLayout,
            .vertexInput = vk::PipelineVertexInputStateCreateInfo{{}, bindings, attributes},
            .topology = vk::PrimitiveTopology::ePatchList,
            .patchControlPoints = 1u,
            .specializationInfo = vk::SpecializationInfo{1u, &SPEC_MAP_ENTRY, sizeof(totalTextures), &totalTextures}
        }, PipelineGraphicsSources{
            .vert = sprite_vert,
            .tesc = sprite_tesc,
            .tese = sprite_tese,
            .frag = sprite_frag
        }
    };
}

}