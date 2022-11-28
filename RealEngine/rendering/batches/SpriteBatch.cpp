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

template<Renderer R>
Glyph<R>::Glyph(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy<R> tex, int depth, Color color) :
    tex(tex),
    depth(depth),
    topLeft({posSize.x, posSize.y + posSize.w}, color, {uv.x, uv.y + uv.w}),
    topRight({posSize.x + posSize.z, posSize.y + posSize.w}, color, {uv.x + uv.z, uv.y + uv.w}),
    botLeft({posSize.x, posSize.y}, color, {uv.x, uv.y}),
    botRight({posSize.x + posSize.z, posSize.y}, color, {uv.x + uv.z, uv.y}) {

}

template<Renderer R>
Glyph<R>::Glyph(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy<R> tex, int depth, Color color, float radAngle, const glm::vec2& origin) :
    tex(tex),
    depth(depth),
    topLeft({glm::vec2{posSize.x, posSize.y} + rotatePoint(glm::vec2(0.0f, posSize.w) - origin, radAngle)}, color, {uv.x, uv.y + uv.w}),
    topRight({glm::vec2{posSize.x, posSize.y} + rotatePoint(glm::vec2(posSize.z, posSize.w) - origin, radAngle)}, color, {uv.x + uv.z, uv.y + uv.w}),
    botLeft({glm::vec2{posSize.x, posSize.y} + rotatePoint(glm::vec2(0.0f, 0.0f) - origin, radAngle)}, color, {uv.x, uv.y}),
    botRight({glm::vec2{posSize.x, posSize.y} + rotatePoint(glm::vec2(posSize.z, 0.0f) - origin, radAngle)}, color, {uv.x + uv.z, uv.y}) {

}

template<Renderer R>
SpriteBatch<R>::SpriteBatch(const ShaderProgramSources& sources) :
    m_pipeline(createVertexInputStateInfo(), sources) {
    m_vertices = m_vbo.map<VertexPOCOUV>(0u, sizeof(VertexPOCOUV) * 512);
}

template<Renderer R>
void SpriteBatch<R>::begin() {
    m_drawBatches.clear();
    m_glyphs.clear();
}

template<Renderer R>
void SpriteBatch<R>::end(GlyphSortType sortType) {
    m_glyphPointers.resize(m_glyphs.size());
    for (size_t i = 0; i < m_glyphs.size(); i++) {
        m_glyphPointers[i] = &m_glyphs[i];
    }
    sortGlyphs(sortType);
    createDrawBatches();
}

//UNCOLORED
template<Renderer R>
void SpriteBatch<R>::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy<R> tex, int depth) {
    m_glyphs.emplace_back(posSize, uv, tex, depth, WHITE);
}
template<Renderer R>
void SpriteBatch<R>::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy<R> tex, int depth, float radAngle, const glm::vec2& origin) {
    m_glyphs.emplace_back(posSize, uv, tex, depth, WHITE, -radAngle, origin);
}
template<Renderer R>
void SpriteBatch<R>::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy<R> tex, int depth, const glm::vec2& direction, const glm::vec2& origin) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(posSize, uv, tex, depth, WHITE, -radAngle, origin);
}
//COLORED
template<Renderer R>
void SpriteBatch<R>::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy<R> tex, int depth, Color color) {
    m_glyphs.emplace_back(posSize, uv, tex, depth, color);
}
template<Renderer R>
void SpriteBatch<R>::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy<R> tex, int depth, Color color, float radAngle, const glm::vec2& origin) {
    m_glyphs.emplace_back(posSize, uv, tex, depth, color, -radAngle, origin);
}
template<Renderer R>
void SpriteBatch<R>::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy<R> tex, int depth, Color color, const glm::vec2& direction, const glm::vec2& origin) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(posSize, uv, tex, depth, color, -radAngle, origin);
}
//UNCOLORED, UNSTRETCHED
template<Renderer R>
void SpriteBatch<R>::addTexture(const Texture<R>& tex, const glm::vec2& position, int depth) {
    m_glyphs.emplace_back(glm::vec4(position - tex.getPivot(), tex.getSubimageDims()), UV_RECT, TextureProxy<R>{tex}, depth, WHITE);
}
template<Renderer R>
void SpriteBatch<R>::addTexture(const Texture<R>& tex, const glm::vec2& position, int depth, float radAngle) {
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims()), UV_RECT, TextureProxy<R>{tex}, depth, WHITE, radAngle, tex.getPivot());
}
template<Renderer R>
void SpriteBatch<R>::addTexture(const Texture<R>& tex, const glm::vec2& position, int depth, const glm::vec2& direction) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims()), UV_RECT, TextureProxy<R>{tex}, depth, WHITE, -radAngle, tex.getPivot());
}
//COLORED, STRETCHED
template<Renderer R>
void SpriteBatch<R>::addTexture(const Texture<R>& tex, const glm::vec2& position, int depth, Color color, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    m_glyphs.emplace_back(glm::vec4(position - tex.getPivot() * scale, tex.getSubimageDims() * scale), UV_RECT, TextureProxy<R>{tex}, depth, color);
}
template<Renderer R>
void SpriteBatch<R>::addTexture(const Texture<R>& tex, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), UV_RECT, TextureProxy<R>{tex}, depth, color, radAngle, tex.getPivot()* scale);
}
template<Renderer R>
void SpriteBatch<R>::addTexture(const Texture<R>& tex, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), UV_RECT, TextureProxy<R>{tex}, depth, color, radAngle, tex.getPivot()* scale);
}
//UNCOLORED, UNSTRETCHED
template<Renderer R>
void SpriteBatch<R>::addSprite(const SpriteStatic<R>& sprite, const glm::vec2& position, int depth) {
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position - tex.getPivot(), tex.getSubimageDims()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, WHITE);
}
template<Renderer R>
void SpriteBatch<R>::addSprite(const SpriteStatic<R>& sprite, const glm::vec2& position, int depth, float radAngle) {
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, WHITE, radAngle, tex.getPivot());
}
template<Renderer R>
void SpriteBatch<R>::addSprite(const SpriteStatic<R>& sprite, const glm::vec2& position, int depth, const glm::vec2& direction) {
    float radAngle = atan2(direction.y, direction.x);
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, WHITE, radAngle, tex.getPivot());
}
//COLORED, STRETCHED
template<Renderer R>
void SpriteBatch<R>::addSprite(const SpriteStatic<R>& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position - tex.getPivot() * scale, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, color);
}
template<Renderer R>
void SpriteBatch<R>::addSprite(const SpriteStatic<R>& sprite, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, color, radAngle, tex.getPivot()* scale);
}
template<Renderer R>
void SpriteBatch<R>::addSprite(const SpriteStatic<R>& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, color, radAngle, tex.getPivot()* scale);
}
//COLORED, STRETCHED BY FULLSPRITE
template<Renderer R>
void SpriteBatch<R>::addSprite(const SpriteComplex<R>& sprite, const glm::vec2& position, int depth) {
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, sprite.getColor(), 0.0f, tex.getPivot()* sprite.getScale());
}
template<Renderer R>
void SpriteBatch<R>::addSprite(const SpriteComplex<R>& sprite, const glm::vec2& position, int depth, float radAngle) {
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, sprite.getColor(), radAngle, tex.getPivot()* sprite.getScale());
}
template<Renderer R>
void SpriteBatch<R>::addSprite(const SpriteComplex<R>& sprite, const glm::vec2& position, int depth, const glm::vec2& direction) {
    float radAngle = atan2(direction.y, direction.x);
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, sprite.getColor(), radAngle, tex.getPivot()* sprite.getScale());
}
//COLORED, STRETCHED BY USER
template<Renderer R>
void SpriteBatch<R>::addSprite(const SpriteComplex<R>& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, color, 0.0f, tex.getPivot()* scale);
}
template<Renderer R>
void SpriteBatch<R>::addSprite(const SpriteComplex<R>& sprite, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, color, radAngle, tex.getPivot()* scale);
}
template<Renderer R>
void SpriteBatch<R>::addSprite(const SpriteComplex<R>& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, color, radAngle, tex.getPivot()* scale);
}
//UNCOLORED, UNSTRETCHED
template<Renderer R>
void SpriteBatch<R>::addSubimage(const Texture<R>& tex, const glm::vec2& position, int depth, const glm::vec2& subImg_Spr) {
    m_glyphs.emplace_back(glm::vec4(position - tex.getPivot(), tex.getSubimageDims()), glm::vec4(glm::floor(subImg_Spr) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, WHITE);
}
template<Renderer R>
void SpriteBatch<R>::addSubimage(const Texture<R>& tex, const glm::vec2& position, int depth, float radAngle, const glm::vec2& subImg_Spr) {
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims()), glm::vec4(glm::floor(subImg_Spr) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, WHITE, radAngle, tex.getPivot());
}
template<Renderer R>
void SpriteBatch<R>::addSubimage(const Texture<R>& tex, const glm::vec2& position, int depth, const glm::vec2& direction, const glm::vec2& subImg_Spr) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims()), glm::vec4(glm::floor(subImg_Spr) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, WHITE, radAngle, tex.getPivot());
}
//COLORED, STRETCHED
template<Renderer R>
void SpriteBatch<R>::addSubimage(const Texture<R>& tex, const glm::vec2& position, int depth, Color color, const glm::vec2& subImg_Spr, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    m_glyphs.emplace_back(glm::vec4(position - tex.getPivot() * scale, tex.getSubimageDims() * scale), glm::vec4(glm::floor(subImg_Spr) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, color);
}
template<Renderer R>
void SpriteBatch<R>::addSubimage(const Texture<R>& tex, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& subImg_Spr, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(subImg_Spr) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, color, radAngle, tex.getPivot()* scale);
}
template<Renderer R>
void SpriteBatch<R>::addSubimage(const Texture<R>& tex, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& subImg_Spr, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(subImg_Spr) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, color, radAngle, tex.getPivot()* scale);
}
//UNCOLORED, UNSTRETCHED
template<Renderer R>
void SpriteBatch<R>::addSurface(const Surface<R>& surface, const glm::vec2& position, int depth, int index) {
    m_glyphs.emplace_back(glm::vec4(position - surface.getPivot(), surface.getDims()), SUV_RECT, surface.getTextureProxy(index), depth, WHITE);
}
template<Renderer R>
void SpriteBatch<R>::addSurface(const Surface<R>& surface, const glm::vec2& position, int depth, int index, float radAngle) {
    m_glyphs.emplace_back(glm::vec4(position, surface.getDims()), SUV_RECT, surface.getTextureProxy(index), depth, WHITE, radAngle, surface.getPivot());
}
template<Renderer R>
void SpriteBatch<R>::addSurface(const Surface<R>& surface, const glm::vec2& position, int depth, int index, const glm::vec2& direction) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, surface.getDims()), SUV_RECT, surface.getTextureProxy(index), depth, WHITE, radAngle, surface.getPivot());
}
//COLORED, STRETCHED
template<Renderer R>
void SpriteBatch<R>::addSurface(const Surface<R>& surface, const glm::vec2& position, int depth, int index, Color color, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    m_glyphs.emplace_back(glm::vec4(position - surface.getPivot() * scale, (glm::vec2)surface.getDims() * scale), SUV_RECT, surface.getTextureProxy(index), depth, color);
}
template<Renderer R>
void SpriteBatch<R>::addSurface(const Surface<R>& surface, const glm::vec2& position, int depth, int index, Color color, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    m_glyphs.emplace_back(glm::vec4(position, (glm::vec2)surface.getDims() * scale), SUV_RECT, surface.getTextureProxy(index), depth, color, radAngle, surface.getPivot() * scale);
}
template<Renderer R>
void SpriteBatch<R>::addSurface(const Surface<R>& surface, const glm::vec2& position, int depth, int index, Color color, const glm::vec2& direction, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, (glm::vec2)surface.getDims() * scale), SUV_RECT, surface.getTextureProxy(index), depth, color, radAngle, surface.getPivot() * scale);
}

template<Renderer R>
void SpriteBatch<R>::draw() {
    draw(m_pipeline);
}

template<Renderer R>
void SpriteBatch<R>::draw(const Pipeline<R>& pipeline) {
    pipeline.bind(vk::PipelineBindPoint::eGraphics);
    for (size_t i = 0u; i < m_drawBatches.size(); i++) {
        m_drawBatches[i].tex.bind();
        pipeline.draw(m_drawBatches[i].count, 1, m_drawBatches[i].offset, 0);
    }
}

template<Renderer R>
void SpriteBatch<R>::changePipeline(const ShaderProgramSources& sources) {
    m_pipeline = Pipeline<R>{createVertexInputStateInfo(), sources};
}

template<Renderer R>
void SpriteBatch<R>::sortGlyphs(GlyphSortType sortType) {
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

template<Renderer R>
void SpriteBatch<R>::createDrawBatches() {
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

template<Renderer R>
vk::PipelineVertexInputStateCreateInfo SpriteBatch<R>::createVertexInputStateInfo() const {
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

template class SpriteBatch<RendererLateBind>;
template class SpriteBatch<RendererVK13>;

}