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
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), UV_RECT, TextureProxy<R>{tex}, depth, color, radAngle, tex.getPivot() * scale);
}
template<Renderer R>
void SpriteBatch<R>::addTexture(const Texture<R>& tex, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), UV_RECT, TextureProxy<R>{tex}, depth, color, radAngle, tex.getPivot() * scale);
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
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, color, radAngle, tex.getPivot() * scale);
}
template<Renderer R>
void SpriteBatch<R>::addSprite(const SpriteStatic<R>& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, color, radAngle, tex.getPivot() * scale);
}
//COLORED, STRETCHED BY FULLSPRITE
template<Renderer R>
void SpriteBatch<R>::addSprite(const SpriteComplex<R>& sprite, const glm::vec2& position, int depth) {
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, sprite.getColor(), 0.0f, tex.getPivot() * sprite.getScale());
}
template<Renderer R>
void SpriteBatch<R>::addSprite(const SpriteComplex<R>& sprite, const glm::vec2& position, int depth, float radAngle) {
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, sprite.getColor(), radAngle, tex.getPivot() * sprite.getScale());
}
template<Renderer R>
void SpriteBatch<R>::addSprite(const SpriteComplex<R>& sprite, const glm::vec2& position, int depth, const glm::vec2& direction) {
    float radAngle = atan2(direction.y, direction.x);
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, sprite.getColor(), radAngle, tex.getPivot() * sprite.getScale());
}
//COLORED, STRETCHED BY USER
template<Renderer R>
void SpriteBatch<R>::addSprite(const SpriteComplex<R>& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, color, 0.0f, tex.getPivot() * scale);
}
template<Renderer R>
void SpriteBatch<R>::addSprite(const SpriteComplex<R>& sprite, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, color, radAngle, tex.getPivot() * scale);
}
template<Renderer R>
void SpriteBatch<R>::addSprite(const SpriteComplex<R>& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    const auto& tex = sprite.getTexture();
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, color, radAngle, tex.getPivot() * scale);
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
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(subImg_Spr) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, color, radAngle, tex.getPivot() * scale);
}
template<Renderer R>
void SpriteBatch<R>::addSubimage(const Texture<R>& tex, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& subImg_Spr, const glm::vec2& scale) {
    float radAngle = atan2(direction.y, direction.x);
    m_glyphs.emplace_back(glm::vec4(position, tex.getSubimageDims() * scale), glm::vec4(glm::floor(subImg_Spr) / tex.getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / tex.getSubimagesSpritesCount()), TextureProxy<R>{tex}, depth, color, radAngle, tex.getPivot() * scale);
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
    draw(m_shaderProgram);
}

template<Renderer R>
void SpriteBatch<R>::draw(const ShaderProgram<R>& program) {
    program.use();
    m_vao.bind();

    for (size_t i = 0u; i < m_drawBatches.size(); i++) {
        m_drawBatches[i].tex.bind();
        m_vao.renderArrays(Primitive::TRIANGLES, m_drawBatches[i].offset, m_drawBatches[i].count);
    }

    m_vao.unbind();
    program.unuse();
}

template<Renderer R>
void SpriteBatch<R>::switchShaderProgram(const ShaderProgramSources& sources) {
    m_shaderProgram = ShaderProgram<R>{sources};
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
    m_vertices.clear();
    if (m_glyphPointers.empty()) {//If there are no glyphs to draw
        return;
    }

    m_vertices.resize(m_glyphPointers.size() * 6u);

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

    //Uploade m_vertices to VBO
    m_vbo.redefine(m_vertices.size() * sizeof(VertexPOCOUV), m_vertices.data());
}

template SpriteBatch<RendererLateBind>;
template SpriteBatch<RendererGL46>;

}