/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/batches/SpriteBatch.hpp>

#include <algorithm>

#include <glm/common.hpp>


glm::vec2 rotatePoint(const glm::vec2& point, float radAngle) {
    return glm::vec2(point.x * cos(radAngle) - point.y * sin(radAngle), point.x * sin(radAngle) + point.y * cos(radAngle));
}

namespace RE {

Glyph::Glyph(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy tex, int depth, Color color) :
    tex(tex),
    depth(depth),
    topLeft({posSize.x, posSize.y + posSize.w}, color, {uv.x, uv.y + uv.w}),
    topRight({posSize.x + posSize.z, posSize.y + posSize.w}, color, {uv.x + uv.z, uv.y + uv.w}),
    botLeft({posSize.x, posSize.y}, color, {uv.x, uv.y}),
    botRight({posSize.x + posSize.z, posSize.y}, color, {uv.x + uv.z, uv.y}) {

}

Glyph::Glyph(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy tex, int depth, Color color, float radAngle, const glm::vec2& origin) :
    tex(tex),
    depth(depth),
    topLeft({glm::vec2{posSize.x, posSize.y} + rotatePoint(glm::vec2(0.0f, posSize.w) - origin, radAngle)}, color, {uv.x, uv.y + uv.w}),
    topRight({glm::vec2{posSize.x, posSize.y} + rotatePoint(glm::vec2(posSize.z, posSize.w) - origin, radAngle)}, color, {uv.x + uv.z, uv.y + uv.w}),
    botLeft({glm::vec2{posSize.x, posSize.y} + rotatePoint(glm::vec2(0.0f, 0.0f) - origin, radAngle)}, color, {uv.x, uv.y}),
    botRight({glm::vec2{posSize.x, posSize.y} + rotatePoint(glm::vec2(posSize.z, 0.0f) - origin, radAngle)}, color, {uv.x + uv.z, uv.y}) {

}

SpriteBatch::SpriteBatch(const PipelineSources& sources) :
    m_pipeline(createVertexInputStateInfo(), {{}, vk::PrimitiveTopology::eTriangleList, false}, sources) {
    m_vertices = m_vbo.map<VertexPOCOUV>(0u, sizeof(VertexPOCOUV) * 512);
}

void SpriteBatch::begin() {
    m_drawBatches.clear();
    m_glyphs.clear();
}

void SpriteBatch::end(GlyphSortType sortType) {
    m_glyphPointers.resize(m_glyphs.size());
    for (size_t i = 0; i < m_glyphs.size(); i++) {
        m_glyphPointers[i] = &m_glyphs[i];
    }
    sortGlyphs(sortType);
    createDrawBatches();
}

//UNCOLORED
void SpriteBatch::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy tex, int depth) {
    m_glyphs.emplace_back(posSize, uv, tex, depth, WHITE);
}
void SpriteBatch::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy tex, int depth, float radAngle, const glm::vec2& origin) {
    m_glyphs.emplace_back(posSize, uv, tex, depth, WHITE, -radAngle, origin);
}
void SpriteBatch::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy tex, int depth, const glm::vec2& direction, const glm::vec2& origin) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(posSize, uv, tex, depth, WHITE, -radAngle, origin);
}
//COLORED
void SpriteBatch::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy tex, int depth, Color color) {
    m_glyphs.emplace_back(posSize, uv, tex, depth, color);
}
void SpriteBatch::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy tex, int depth, Color color, float radAngle, const glm::vec2& origin) {
    m_glyphs.emplace_back(posSize, uv, tex, depth, color, -radAngle, origin);
}
void SpriteBatch::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy tex, int depth, Color color, const glm::vec2& direction, const glm::vec2& origin) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(posSize, uv, tex, depth, color, -radAngle, origin);
}
//UNCOLORED, UNSTRETCHED
void SpriteBatch::addTexture(const Texture& tex, const glm::vec2& position, int depth) {
    m_glyphs.emplace_back(glm::vec4(position - tex.getPivot(), tex.getSubimageDims()), UV_RECT, TextureProxy{tex}, depth, WHITE);
}
void SpriteBatch::addTexture(const Texture& tex, const glm::vec2& position, int depth, float radAngle) {
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims()), UV_RECT, TextureProxy{tex}, depth, WHITE, radAngle, tex.getPivot());
}
void SpriteBatch::addTexture(const Texture& tex, const glm::vec2& position, int depth, const glm::vec2& direction) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims()), UV_RECT, TextureProxy{tex}, depth, WHITE, -radAngle, tex.getPivot());
}
//COLORED, STRETCHED
void SpriteBatch::addTexture(const Texture& tex, const glm::vec2& position, int depth, Color color, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    m_glyphs.emplace_back(glm::vec4(position - tex.getPivot() * scale, tex.getSubimageDims() * scale), UV_RECT, TextureProxy{tex}, depth, color);
}
void SpriteBatch::addTexture(const Texture& tex, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), UV_RECT, TextureProxy{tex}, depth, color, radAngle, tex.getPivot() * scale);
}
void SpriteBatch::addTexture(const Texture& tex, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), UV_RECT, TextureProxy{tex}, depth, color, radAngle, tex.getPivot() * scale);
}
//UNCOLORED, UNSTRETCHED
void SpriteBatch::addSprite(const SpriteStatic& sprite, const glm::vec2& position, int depth) {
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position - tex.getPivot(), tex.getSubimageDims()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, WHITE);
}
void SpriteBatch::addSprite(const SpriteStatic& sprite, const glm::vec2& position, int depth, float radAngle) {
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, WHITE, radAngle, tex.getPivot());
}
void SpriteBatch::addSprite(const SpriteStatic& sprite, const glm::vec2& position, int depth, const glm::vec2& direction) {
    float radAngle = atan2(direction.y, direction.x);
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, WHITE, radAngle, tex.getPivot());
}
//COLORED, STRETCHED
void SpriteBatch::addSprite(const SpriteStatic& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position - tex.getPivot() * scale, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, color);
}
void SpriteBatch::addSprite(const SpriteStatic& sprite, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, color, radAngle, tex.getPivot() * scale);
}
void SpriteBatch::addSprite(const SpriteStatic& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, color, radAngle, tex.getPivot() * scale);
}
//COLORED, STRETCHED BY FULLSPRITE
void SpriteBatch::addSprite(const SpriteComplex& sprite, const glm::vec2& position, int depth) {
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, sprite.getColor(), 0.0f, tex.getPivot() * sprite.getScale());
}
void SpriteBatch::addSprite(const SpriteComplex& sprite, const glm::vec2& position, int depth, float radAngle) {
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, sprite.getColor(), radAngle, tex.getPivot() * sprite.getScale());
}
void SpriteBatch::addSprite(const SpriteComplex& sprite, const glm::vec2& position, int depth, const glm::vec2& direction) {
    float radAngle = atan2(direction.y, direction.x);
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, sprite.getColor(), radAngle, tex.getPivot() * sprite.getScale());
}
//COLORED, STRETCHED BY USER
void SpriteBatch::addSprite(const SpriteComplex& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, color, 0.0f, tex.getPivot() * scale);
}
void SpriteBatch::addSprite(const SpriteComplex& sprite, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, color, radAngle, tex.getPivot() * scale);
}
void SpriteBatch::addSprite(const SpriteComplex& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, color, radAngle, tex.getPivot() * scale);
}
//UNCOLORED, UNSTRETCHED
void SpriteBatch::addSubimage(const Texture& tex, const glm::vec2& position, int depth, const glm::vec2& subImg_Spr) {
    m_glyphs.emplace_back(glm::vec4(position - tex.getPivot(), tex.getSubimageDims()), glm::vec4(glm::floor(subImg_Spr) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, WHITE);
}
void SpriteBatch::addSubimage(const Texture& tex, const glm::vec2& position, int depth, float radAngle, const glm::vec2& subImg_Spr) {
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims()), glm::vec4(glm::floor(subImg_Spr) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, WHITE, radAngle, tex.getPivot());
}
void SpriteBatch::addSubimage(const Texture& tex, const glm::vec2& position, int depth, const glm::vec2& direction, const glm::vec2& subImg_Spr) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims()), glm::vec4(glm::floor(subImg_Spr) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, WHITE, radAngle, tex.getPivot());
}
//COLORED, STRETCHED
void SpriteBatch::addSubimage(const Texture& tex, const glm::vec2& position, int depth, Color color, const glm::vec2& subImg_Spr, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    m_glyphs.emplace_back(glm::vec4(position - tex.getPivot() * scale, tex.getSubimageDims() * scale), glm::vec4(glm::floor(subImg_Spr) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, color);
}
void SpriteBatch::addSubimage(const Texture& tex, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& subImg_Spr, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(subImg_Spr) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, color, radAngle, tex.getPivot() * scale);
}
void SpriteBatch::addSubimage(const Texture& tex, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& subImg_Spr, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(subImg_Spr) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy{tex}, depth, color, radAngle, tex.getPivot() * scale);
}

void SpriteBatch::draw(const vk::ArrayProxyNoTemporaries<DescriptorSet>& descriptorSets) {
    draw(descriptorSets, m_pipeline);
}

void SpriteBatch::draw(const vk::ArrayProxyNoTemporaries<DescriptorSet>& descriptorSets, const Pipeline& pipeline) {
    pipeline.bind(vk::PipelineBindPoint::eGraphics);
    m_vbo.bindAsVertexBuffer(0u, 0u);
    for (const auto& set : descriptorSets) {
        set.bind(vk::PipelineBindPoint::eGraphics, m_pipeline);
    }
    //TODO
    for (size_t i = 0u; i < m_drawBatches.size(); i++) {
        pipeline.draw(m_drawBatches[i].count, 1, m_drawBatches[i].offset, 0);
    }
}

void SpriteBatch::changePipeline(const PipelineSources& sources) {
    m_pipeline = Pipeline{createVertexInputStateInfo(), {{}, vk::PrimitiveTopology::eTriangleList, false}, sources};
}

void SpriteBatch::sortGlyphs(GlyphSortType sortType) {
    switch (sortType) {
    case GlyphSortType::NEG_TOP:
        std::stable_sort(m_glyphPointers.begin(), m_glyphPointers.end(), compareNegToPos);
        break;
    case GlyphSortType::POS_TOP:
        std::stable_sort(m_glyphPointers.begin(), m_glyphPointers.end(), comparePosToNeg);
        break;
    case GlyphSortType::TEXTURE:
        std::stable_sort(m_glyphPointers.begin(), m_glyphPointers.end(), compareTexture);
        break;
    }
}

void SpriteBatch::createDrawBatches() {
    if (m_glyphPointers.empty()) {//If there are no glyphs to draw
        return;
    }

    assert(m_glyphPointers.size() * 6u < 512);

    int currentVertex = 0;
    int offset = 0;

    m_drawBatches.emplace_back(offset, 6, m_glyphPointers[0]->tex);
    m_vertices[currentVertex++] = m_glyphPointers[0]->topLeft;
    m_vertices[currentVertex++] = m_glyphPointers[0]->botLeft;
    m_vertices[currentVertex++] = m_glyphPointers[0]->topRight;
    m_vertices[currentVertex++] = m_glyphPointers[0]->topRight;
    m_vertices[currentVertex++] = m_glyphPointers[0]->botLeft;
    m_vertices[currentVertex++] = m_glyphPointers[0]->botRight;

    offset += 6;

    for (size_t i = 1; i < m_glyphPointers.size(); i++) {
        if (m_glyphPointers[i]->tex != m_glyphPointers[i - 1]->tex) {
            m_drawBatches.emplace_back(offset, 6, m_glyphPointers[i]->tex);
        } else {
            m_drawBatches.back().count += 6;
        }
        m_vertices[currentVertex++] = m_glyphPointers[i]->topLeft;
        m_vertices[currentVertex++] = m_glyphPointers[i]->botLeft;
        m_vertices[currentVertex++] = m_glyphPointers[i]->topRight;
        m_vertices[currentVertex++] = m_glyphPointers[i]->topRight;
        m_vertices[currentVertex++] = m_glyphPointers[i]->botLeft;
        m_vertices[currentVertex++] = m_glyphPointers[i]->botRight;
        offset += 6;
    }
}

vk::PipelineVertexInputStateCreateInfo SpriteBatch::createVertexInputStateInfo() const {
    static constexpr std::array bindings = std::to_array<vk::VertexInputBindingDescription>({{
        0u,                             //Binding index
        sizeof(VertexPOCOUV),           //Stride
        vk::VertexInputRate::eVertex    //Input rate
    }});
    static constexpr std::array attributes = std::to_array<vk::VertexInputAttributeDescription>({{
        ATTR_POSITION,                  //Location
        0u,                             //Binding index
        vk::Format::eR32G32B32A32Sfloat,//Format
        offsetof(VertexPOCOUV, position)//Relative offset
    },{
        ATTR_COLOR,                     //Location
        0u,                             //Binding index
        vk::Format::eR8G8B8A8Unorm,     //Format
        offsetof(VertexPOCOUV, color)   //Relative offset
    },{
        ATTR_UV,                        //Location
        0u,                             //Binding index
        vk::Format::eR32G32Sfloat,      //Format
        offsetof(VertexPOCOUV, uv)      //Relative offset
    }});
    return vk::PipelineVertexInputStateCreateInfo{{}, bindings, attributes};
}

}