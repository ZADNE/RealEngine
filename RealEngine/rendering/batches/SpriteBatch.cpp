/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/batches/SpriteBatch.hpp>

#include <algorithm>

#include <glm/common.hpp>

#include <RealEngine/rendering/batches/shaders/AllShaders.hpp>
#include <RealEngine/rendering/PerFrameInFlight.hpp>

using enum vk::DescriptorBindingFlagBits;

namespace RE {

glm::vec2 rotatePoint(const glm::vec2& point, float radAngle) {
    return glm::vec2(point.x * cos(radAngle) - point.y * sin(radAngle), point.x * sin(radAngle) + point.y * cos(radAngle));
}

SpriteBatch::SpriteBatch(int maxSprites, int maxTextures) :
    m_spritesBuf(MAX_FRAMES_IN_FLIGHT* maxSprites * sizeof(Sprite), eVertexBuffer, eHostVisible | eHostCoherent),
    m_spritesMapped(m_spritesBuf.map<Sprite>(0u, MAX_FRAMES_IN_FLIGHT* maxSprites * sizeof(Sprite))),
    m_maxSprites(maxSprites),
    m_maxTextures(maxTextures),
    m_pipeline(createPipeline(maxTextures)) {
}

void SpriteBatch::begin() {
    m_nextSpriteIndex = m_maxSprites * NEXT_FRAME;
}

void SpriteBatch::end() {
    //Nothing to do :-)
}

void SpriteBatch::draw(const vk::CommandBuffer& commandBuffer, const glm::mat4& mvpMat) {
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *m_pipeline);
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipeline.pipelineLayout(), 0u, *m_descSet, {});
    commandBuffer.bindVertexBuffers(0u, *m_spritesBuf, 0ull);
    commandBuffer.pushConstants<glm::mat4>(m_pipeline.pipelineLayout(), vk::ShaderStageFlagBits::eTessellationEvaluation, 0u, mvpMat);
    commandBuffer.draw(m_nextSpriteIndex - m_maxSprites * NEXT_FRAME, 1u, m_maxSprites * CURRENT_FRAME, 0u);
}

//UNCOLORED
void SpriteBatch::add(const glm::vec4& posSize, const glm::vec4& uv, const Texture& tex, int depth) {
    glm::uint texIndex = m_maxTextures * NEXT_FRAME;
    m_spritesMapped[m_nextSpriteIndex++] = Sprite{.pos = posSize, .uvs = uv, .tex = texIndex, .col = WHITE};
    m_descSet.write(vk::DescriptorType::eCombinedImageSampler, 0u, texIndex, tex);
}
void SpriteBatch::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy tex, int depth, float radAngle, const glm::vec2& origin) {
    //m_glyphs.emplace_back(posSize, uv, tex, depth, WHITE, -radAngle, origin);
}
void SpriteBatch::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy tex, int depth, const glm::vec2& direction, const glm::vec2& origin) {
    float radAngle = atan2(direction.y, direction.x);
    //m_glyphs.emplace_back(posSize, uv, tex, depth, WHITE, -radAngle, origin);
}
//COLORED
void SpriteBatch::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy tex, int depth, Color color) {
    //m_glyphs.emplace_back(posSize, uv, tex, depth, color);
}
void SpriteBatch::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy tex, int depth, Color color, float radAngle, const glm::vec2& origin) {
    //m_glyphs.emplace_back(posSize, uv, tex, depth, color, -radAngle, origin);
}
void SpriteBatch::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy tex, int depth, Color color, const glm::vec2& direction, const glm::vec2& origin) {
    float radAngle = atan2(direction.y, direction.x);
    //m_glyphs.emplace_back(posSize, uv, tex, depth, color, -radAngle, origin);
}
//UNCOLORED, UNSTRETCHED
void SpriteBatch::addTexture(const Texture& tex, const glm::vec2& position, int depth) {
    //m_glyphs.emplace_back(glm::vec4(position - tex.getPivot(), tex.getSubimageDims()), UV_RECT, TextureProxy{tex}, depth, WHITE);
}
void SpriteBatch::addTexture(const Texture& tex, const glm::vec2& position, int depth, float radAngle) {
    //m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims()), UV_RECT, TextureProxy{tex}, depth, WHITE, radAngle, tex.getPivot());
}
void SpriteBatch::addTexture(const Texture& tex, const glm::vec2& position, int depth, const glm::vec2& direction) {
    float radAngle = atan2(direction.y, direction.x);
    //m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims()), UV_RECT, TextureProxy{tex}, depth, WHITE, -radAngle, tex.getPivot());
}
//COLORED, STRETCHED
void SpriteBatch::addTexture(const Texture& tex, const glm::vec2& position, int depth, Color color, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    //m_glyphs.emplace_back(glm::vec4(position - tex.getPivot() * scale, tex.getSubimageDims() * scale), UV_RECT, TextureProxy{tex}, depth, color);
}
void SpriteBatch::addTexture(const Texture& tex, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    //m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), UV_RECT, TextureProxy{tex}, depth, color, radAngle, tex.getPivot() * scale);
}
void SpriteBatch::addTexture(const Texture& tex, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    //m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), UV_RECT, TextureProxy{tex}, depth, color, radAngle, tex.getPivot() * scale);
}
//UNCOLORED, UNSTRETCHED
void SpriteBatch::addSprite(const SpriteStatic& sprite, const glm::vec2& position, int depth) {
    const auto& tex = sprite.getTexture();
    //m_glyphs.emplace_back(glm::vec4(position - tex.getPivot(), tex.getSubimageDims()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, WHITE);
}
void SpriteBatch::addSprite(const SpriteStatic& sprite, const glm::vec2& position, int depth, float radAngle) {
    const auto& tex = sprite.getTexture();
    //m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, WHITE, radAngle, tex.getPivot());
}
void SpriteBatch::addSprite(const SpriteStatic& sprite, const glm::vec2& position, int depth, const glm::vec2& direction) {
    float radAngle = atan2(direction.y, direction.x);
    const auto& tex = sprite.getTexture();
    //m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, WHITE, radAngle, tex.getPivot());
}
//COLORED, STRETCHED
void SpriteBatch::addSprite(const SpriteStatic& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    const auto& tex = sprite.getTexture();
    //m_glyphs.emplace_back(glm::vec4(position - tex.getPivot() * scale, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, color);
}
void SpriteBatch::addSprite(const SpriteStatic& sprite, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    const auto& tex = sprite.getTexture();
    //m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, color, radAngle, tex.getPivot() * scale);
}
void SpriteBatch::addSprite(const SpriteStatic& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    const auto& tex = sprite.getTexture();
    //m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, color, radAngle, tex.getPivot() * scale);
}
//COLORED, STRETCHED BY FULLSPRITE
void SpriteBatch::addSprite(const SpriteComplex& sprite, const glm::vec2& position, int depth) {
    const auto& tex = sprite.getTexture();
    //m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, sprite.getColor(), 0.0f, tex.getPivot() * sprite.getScale());
}
void SpriteBatch::addSprite(const SpriteComplex& sprite, const glm::vec2& position, int depth, float radAngle) {
    const auto& tex = sprite.getTexture();
    //m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, sprite.getColor(), radAngle, tex.getPivot() * sprite.getScale());
}
void SpriteBatch::addSprite(const SpriteComplex& sprite, const glm::vec2& position, int depth, const glm::vec2& direction) {
    float radAngle = atan2(direction.y, direction.x);
    const auto& tex = sprite.getTexture();
    //m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, sprite.getColor(), radAngle, tex.getPivot() * sprite.getScale());
}
//COLORED, STRETCHED BY USER
void SpriteBatch::addSprite(const SpriteComplex& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    const auto& tex = sprite.getTexture();
    //m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, color, 0.0f, tex.getPivot() * scale);
}
void SpriteBatch::addSprite(const SpriteComplex& sprite, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    const auto& tex = sprite.getTexture();
    //m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, color, radAngle, tex.getPivot() * scale);
}
void SpriteBatch::addSprite(const SpriteComplex& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    const auto& tex = sprite.getTexture();
    //m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, color, radAngle, tex.getPivot() * scale);
}
//UNCOLORED, UNSTRETCHED
void SpriteBatch::addSubimage(const Texture& tex, const glm::vec2& position, int depth, const glm::vec2& subImg_Spr) {
    //m_glyphs.emplace_back(glm::vec4(position - tex.getPivot(), tex.getSubimageDims()), glm::vec4(glm::floor(subImg_Spr) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, WHITE);
}
void SpriteBatch::addSubimage(const Texture& tex, const glm::vec2& position, int depth, float radAngle, const glm::vec2& subImg_Spr) {
    //m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims()), glm::vec4(glm::floor(subImg_Spr) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, WHITE, radAngle, tex.getPivot());
}
void SpriteBatch::addSubimage(const Texture& tex, const glm::vec2& position, int depth, const glm::vec2& direction, const glm::vec2& subImg_Spr) {
    float radAngle = atan2(direction.y, direction.x);
    //m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims()), glm::vec4(glm::floor(subImg_Spr) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, WHITE, radAngle, tex.getPivot());
}
//COLORED, STRETCHED
void SpriteBatch::addSubimage(const Texture& tex, const glm::vec2& position, int depth, Color color, const glm::vec2& subImg_Spr, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    //m_glyphs.emplace_back(glm::vec4(position - tex.getPivot() * scale, tex.getSubimageDims() * scale), glm::vec4(glm::floor(subImg_Spr) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, color);
}
void SpriteBatch::addSubimage(const Texture& tex, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& subImg_Spr, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    //m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(subImg_Spr) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, color, radAngle, tex.getPivot() * scale);
}
void SpriteBatch::addSubimage(const Texture& tex, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& subImg_Spr, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    //m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(subImg_Spr) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, color, radAngle, tex.getPivot() * scale);
}

Pipeline SpriteBatch::createPipeline(int maxTextures) const {
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
    int totalTextures = maxTextures * MAX_FRAMES_IN_FLIGHT;
    return Pipeline{
        PipelineCreateInfo{
            .vertexInput = vk::PipelineVertexInputStateCreateInfo{{}, bindings, attributes},
            .topology = vk::PrimitiveTopology::ePatchList,
            .patchControlPoints = 1u,
            .descriptorBindingFlags = {eUpdateUnusedWhilePending | ePartiallyBound},
            .specializationInfo = vk::SpecializationInfo{1u, &SPEC_MAP_ENTRY, sizeof(totalTextures), &totalTextures}
        }, PipelineSources{
            .vert = sprite_vert,
            .tesc = sprite_tesc,
            .tese = sprite_tese,
            .frag = sprite_frag
        }
    };
}

}