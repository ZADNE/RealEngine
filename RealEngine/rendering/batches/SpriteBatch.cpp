﻿/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/batches/SpriteBatch.hpp>

#include <algorithm>

#include <glm/common.hpp>

#include <RealEngine/resources/ResourceManager.hpp>

#include <RealEngine/rendering/RendererLateBind.hpp>
#include <RealEngine/rendering/RendererGL46.hpp>


glm::vec2 rotatePoint(const glm::vec2& point, float radAngle) {
    return glm::vec2(point.x * cos(radAngle) - point.y * sin(radAngle), point.x * sin(radAngle) + point.y * cos(radAngle));
}

namespace RE {

Glyph::Glyph(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy texture, int depth, Color color) :
    texture(texture),
    depth(depth),
    topLeft({posSize.x, posSize.y + posSize.w}, color, {uv.x, uv.y + uv.w}),
    topRight({posSize.x + posSize.z, posSize.y + posSize.w}, color, {uv.x + uv.z, uv.y + uv.w}),
    botLeft({posSize.x, posSize.y}, color, {uv.x, uv.y}),
    botRight({posSize.x + posSize.z, posSize.y}, color, {uv.x + uv.z, uv.y}) {

}

Glyph::Glyph(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy texture, int depth, Color color, float radAngle, const glm::vec2& origin) :
    texture(texture),
    depth(depth),
    topLeft({glm::vec2{posSize.x, posSize.y} + rotatePoint(glm::vec2(0.0f, posSize.w) - origin, radAngle)}, color, {uv.x, uv.y + uv.w}),
    topRight({glm::vec2{posSize.x, posSize.y} + rotatePoint(glm::vec2(posSize.z, posSize.w) - origin, radAngle)}, color, {uv.x + uv.z, uv.y + uv.w}),
    botLeft({glm::vec2{posSize.x, posSize.y} + rotatePoint(glm::vec2(0.0f, 0.0f) - origin, radAngle)}, color, {uv.x, uv.y}),
    botRight({glm::vec2{posSize.x, posSize.y} + rotatePoint(glm::vec2(posSize.z, 0.0f) - origin, radAngle)}, color, {uv.x + uv.z, uv.y}) {

}

template<typename R>
SpriteBatch<R>::SpriteBatch(const ShaderProgramSources& sources):
    m_shaderProgram(sources) {
    unsigned int vboBindingPoint = 0u;
    m_vao.setBindingPoint(vboBindingPoint, m_vbo, 0u, sizeof(VertexPOCOUV));

    m_vao.setAttribute(ATTR_POSITION, VertexComponentCount::XY, VertexComponentType::FLOAT, offsetof(VertexPOCOUV, position));
    m_vao.setAttribute(ATTR_COLOR, VertexComponentCount::RGBA, VertexComponentType::UNSIGNED_BYTE, offsetof(VertexPOCOUV, color));
    m_vao.setAttribute(ATTR_UV, VertexComponentCount::XY, VertexComponentType::FLOAT, offsetof(VertexPOCOUV, uv));

    m_vao.connectAttributeToBindingPoint(ATTR_POSITION, vboBindingPoint);
    m_vao.connectAttributeToBindingPoint(ATTR_COLOR, vboBindingPoint);
    m_vao.connectAttributeToBindingPoint(ATTR_UV, vboBindingPoint);
}

template<typename R>
void SpriteBatch<R>::begin() {
    m_drawBatches.clear();
    m_glyphs.clear();
}

template<typename R>
void SpriteBatch<R>::end(GlyphSortType sortType) {
    m_glyphPointers.resize(m_glyphs.size());
    for (size_t i = 0; i < m_glyphs.size(); i++) {
        m_glyphPointers[i] = &m_glyphs[i];
    }
    sortGlyphs(sortType);
    createDrawBatches();
}

//UNCOLORED
template<typename R>
void SpriteBatch<R>::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy texture, int depth) {
    m_glyphs.emplace_back(posSize, uv, texture, depth, WHITE);
}
template<typename R>
void SpriteBatch<R>::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy texture, int depth, float radAngle, const glm::vec2& origin) {
    m_glyphs.emplace_back(posSize, uv, texture, depth, WHITE, -radAngle, origin);
}
template<typename R>
void SpriteBatch<R>::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy texture, int depth, const glm::vec2& direction, const glm::vec2& origin) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(posSize, uv, texture, depth, WHITE, -radAngle, origin);
}
//COLORED
template<typename R>
void SpriteBatch<R>::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy texture, int depth, Color color) {
    m_glyphs.emplace_back(posSize, uv, texture, depth, color);
}
template<typename R>
void SpriteBatch<R>::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy texture, int depth, Color color, float radAngle, const glm::vec2& origin) {
    m_glyphs.emplace_back(posSize, uv, texture, depth, color, -radAngle, origin);
}
template<typename R>
void SpriteBatch<R>::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy texture, int depth, Color color, const glm::vec2& direction, const glm::vec2& origin) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(posSize, uv, texture, depth, color, -radAngle, origin);
}
//UNCOLORED, UNSTRETCHED
template<typename R>
void SpriteBatch<R>::addTexture(const Texture* texture, const glm::vec2& position, int depth) {
    m_glyphs.emplace_back(glm::vec4(position - texture->getPivot(), texture->getSubimageDims()), UV_RECT, TextureProxy{*texture}, depth, WHITE);
}
template<typename R>
void SpriteBatch<R>::addTexture(const Texture* texture, const glm::vec2& position, int depth, float radAngle) {
    m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims()), UV_RECT, TextureProxy{*texture}, depth, WHITE, radAngle, texture->getPivot());
}
template<typename R>
void SpriteBatch<R>::addTexture(const Texture* texture, const glm::vec2& position, int depth, const glm::vec2& direction) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims()), UV_RECT, TextureProxy{*texture}, depth, WHITE, -radAngle, texture->getPivot());
}
//COLORED, STRETCHED
template<typename R>
void SpriteBatch<R>::addTexture(const Texture* texture, const glm::vec2& position, int depth, Color color, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    m_glyphs.emplace_back(glm::vec4(position - texture->getPivot() * scale, texture->getSubimageDims() * scale), UV_RECT, TextureProxy{*texture}, depth, color);
}
template<typename R>
void SpriteBatch<R>::addTexture(const Texture* texture, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims() * scale), UV_RECT, TextureProxy{*texture}, depth, color, radAngle, texture->getPivot() * scale);
}
template<typename R>
void SpriteBatch<R>::addTexture(const Texture* texture, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims() * scale), UV_RECT, TextureProxy{*texture}, depth, color, radAngle, texture->getPivot() * scale);
}
//UNCOLORED, UNSTRETCHED
template<typename R>
void SpriteBatch<R>::addSprite(const Sprite& sprite, const glm::vec2& position, int depth) {
    m_glyphs.emplace_back(glm::vec4(position - sprite.getTexture()->getPivot(), sprite.getTexture()->getSubimageDims()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, WHITE);
}
template<typename R>
void SpriteBatch<R>::addSprite(const Sprite& sprite, const glm::vec2& position, int depth, float radAngle) {
    m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, WHITE, radAngle, sprite.getTexture()->getPivot());
}
template<typename R>
void SpriteBatch<R>::addSprite(const Sprite& sprite, const glm::vec2& position, int depth, const glm::vec2& direction) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, WHITE, radAngle, sprite.getTexture()->getPivot());
}
//COLORED, STRETCHED
template<typename R>
void SpriteBatch<R>::addSprite(const Sprite& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    m_glyphs.emplace_back(glm::vec4(position - sprite.getTexture()->getPivot() * scale, sprite.getTexture()->getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, color);
}
template<typename R>
void SpriteBatch<R>::addSprite(const Sprite& sprite, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, color, radAngle, sprite.getTexture()->getPivot() * scale);
}
template<typename R>
void SpriteBatch<R>::addSprite(const Sprite& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, color, radAngle, sprite.getTexture()->getPivot() * scale);
}
//COLORED, STRETCHED BY FULLSPRITE
template<typename R>
void SpriteBatch<R>::addSprite(const FullSprite& sprite, const glm::vec2& position, int depth) {
    m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, sprite.getColor(), 0.0f, sprite.getTexture()->getPivot() * sprite.getScale());
}
template<typename R>
void SpriteBatch<R>::addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, float radAngle) {
    m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, sprite.getColor(), radAngle, sprite.getTexture()->getPivot() * sprite.getScale());
}
template<typename R>
void SpriteBatch<R>::addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, const glm::vec2& direction) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, sprite.getColor(), radAngle, sprite.getTexture()->getPivot() * sprite.getScale());
}
//COLORED, STRETCHED BY USER
template<typename R>
void SpriteBatch<R>::addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, color, 0.0f, sprite.getTexture()->getPivot() * scale);
}
template<typename R>
void SpriteBatch<R>::addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, color, radAngle, sprite.getTexture()->getPivot() * scale);
}
template<typename R>
void SpriteBatch<R>::addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, color, radAngle, sprite.getTexture()->getPivot() * scale);
}
//UNCOLORED, UNSTRETCHED
template<typename R>
void SpriteBatch<R>::addSubimage(const Texture* texture, const glm::vec2& position, int depth, const glm::vec2& subImg_Spr) {
    m_glyphs.emplace_back(glm::vec4(position - texture->getPivot(), texture->getSubimageDims()), glm::vec4(glm::floor(subImg_Spr) / texture->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / texture->getSubimagesSpritesCount()), TextureProxy{*texture}, depth, WHITE);
}
template<typename R>
void SpriteBatch<R>::addSubimage(const Texture* texture, const glm::vec2& position, int depth, float radAngle, const glm::vec2& subImg_Spr) {
    m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims()), glm::vec4(glm::floor(subImg_Spr) / texture->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / texture->getSubimagesSpritesCount()), TextureProxy{*texture}, depth, WHITE, radAngle, texture->getPivot());
}
template<typename R>
void SpriteBatch<R>::addSubimage(const Texture* texture, const glm::vec2& position, int depth, const glm::vec2& direction, const glm::vec2& subImg_Spr) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims()), glm::vec4(glm::floor(subImg_Spr) / texture->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / texture->getSubimagesSpritesCount()), TextureProxy{*texture}, depth, WHITE, radAngle, texture->getPivot());
}
//COLORED, STRETCHED
template<typename R>
void SpriteBatch<R>::addSubimage(const Texture* texture, const glm::vec2& position, int depth, Color color, const glm::vec2& subImg_Spr, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    m_glyphs.emplace_back(glm::vec4(position - texture->getPivot() * scale, texture->getSubimageDims() * scale), glm::vec4(glm::floor(subImg_Spr) / texture->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / texture->getSubimagesSpritesCount()), TextureProxy{*texture}, depth, color);
}
template<typename R>
void SpriteBatch<R>::addSubimage(const Texture* texture, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& subImg_Spr, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims() * scale), glm::vec4(glm::floor(subImg_Spr) / texture->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / texture->getSubimagesSpritesCount()), TextureProxy{*texture}, depth, color, radAngle, texture->getPivot() * scale);
}
template<typename R>
void SpriteBatch<R>::addSubimage(const Texture* texture, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& subImg_Spr, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims() * scale), glm::vec4(glm::floor(subImg_Spr) / texture->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / texture->getSubimagesSpritesCount()), TextureProxy{*texture}, depth, color, radAngle, texture->getPivot() * scale);
}
//UNCOLORED, UNSTRETCHED
template<typename R>
void SpriteBatch<R>::addSurface(const Surface<R>& surface, const glm::vec2& position, int depth, int index) {
    m_glyphs.emplace_back(glm::vec4(position - surface.getPivot(), surface.getDims()), SUV_RECT, surface.getTextureProxy(index), depth, WHITE);
}
template<typename R>
void SpriteBatch<R>::addSurface(const Surface<R>& surface, const glm::vec2& position, int depth, int index, float radAngle) {
    m_glyphs.emplace_back(glm::vec4(position, surface.getDims()), SUV_RECT, surface.getTextureProxy(index), depth, WHITE, radAngle, surface.getPivot());
}
template<typename R>
void SpriteBatch<R>::addSurface(const Surface<R>& surface, const glm::vec2& position, int depth, int index, const glm::vec2& direction) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, surface.getDims()), SUV_RECT, surface.getTextureProxy(index), depth, WHITE, radAngle, surface.getPivot());
}
//COLORED, STRETCHED
template<typename R>
void SpriteBatch<R>::addSurface(const Surface<R>& surface, const glm::vec2& position, int depth, int index, Color color, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    m_glyphs.emplace_back(glm::vec4(position - surface.getPivot() * scale, (glm::vec2)surface.getDims() * scale), SUV_RECT, surface.getTextureProxy(index), depth, color);
}
template<typename R>
void SpriteBatch<R>::addSurface(const Surface<R>& surface, const glm::vec2& position, int depth, int index, Color color, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    m_glyphs.emplace_back(glm::vec4(position, (glm::vec2)surface.getDims() * scale), SUV_RECT, surface.getTextureProxy(index), depth, color, radAngle, surface.getPivot() * scale);
}
template<typename R>
void SpriteBatch<R>::addSurface(const Surface<R>& surface, const glm::vec2& position, int depth, int index, Color color, const glm::vec2& direction, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, (glm::vec2)surface.getDims() * scale), SUV_RECT, surface.getTextureProxy(index), depth, color, radAngle, surface.getPivot() * scale);
}

template<typename R>
void SpriteBatch<R>::draw() {
    draw(m_shaderProgram);
}

template<typename R>
void SpriteBatch<R>::draw(const ShaderProgram<R>& program) {
    program.use();
    m_vao.bind();

    for (size_t i = 0u; i < m_drawBatches.size(); i++) {
        m_drawBatches[i].texture.bind();
        m_vao.renderArrays(Primitive::TRIANGLES, m_drawBatches[i].offset, m_drawBatches[i].count);
    }

    m_vao.unbind();
    program.unuse();
}

template<typename R>
void SpriteBatch<R>::switchShaderProgram(const ShaderProgramSources& sources) {
    m_shaderProgram = ShaderProgram<R>{sources};
}

template<typename R>
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

template<typename R>
void SpriteBatch<R>::createDrawBatches() {
    m_vertices.clear();
    if (m_glyphPointers.empty()) {//If there are no glyphs to draw
        return;
    }

    m_vertices.resize(m_glyphPointers.size() * 6u);

    int currentVertex = 0;
    int offset = 0;

    m_drawBatches.emplace_back(offset, 6, m_glyphPointers[0]->texture);
    m_vertices[currentVertex++] = m_glyphPointers[0]->topLeft;
    m_vertices[currentVertex++] = m_glyphPointers[0]->botLeft;
    m_vertices[currentVertex++] = m_glyphPointers[0]->topRight;
    m_vertices[currentVertex++] = m_glyphPointers[0]->topRight;
    m_vertices[currentVertex++] = m_glyphPointers[0]->botLeft;
    m_vertices[currentVertex++] = m_glyphPointers[0]->botRight;

    offset += 6;

    for (size_t i = 1; i < m_glyphPointers.size(); i++) {
        if (m_glyphPointers[i]->texture != m_glyphPointers[i - 1]->texture) {
            m_drawBatches.emplace_back(offset, 6, m_glyphPointers[i]->texture);
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

    //Uploade m_vertices to VBO
    m_vbo.redefine(m_vertices.size() * sizeof(VertexPOCOUV), m_vertices.data());
}

template<typename R>
bool SpriteBatch<R>::compareNegToPos(Glyph* a, Glyph* b) {
    return (a->depth > b->depth);
}

template<typename R>
bool SpriteBatch<R>::comparePosToNeg(Glyph* a, Glyph* b) {
    return (a->depth < b->depth);
}

template<typename R>
bool SpriteBatch<R>::compareTexture(Glyph* a, Glyph* b) {
    return (a->texture > b->texture);
}

template SpriteBatch<RendererLateBind>;
template SpriteBatch<RendererGL46>;

}