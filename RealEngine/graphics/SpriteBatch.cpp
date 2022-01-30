#include <RealEngine/graphics/SpriteBatch.hpp>

#include <algorithm>

#include <glm/common.hpp>

#include <RealEngine/resources/ResourceManager.hpp>


glm::vec2 rotatePoint(const glm::vec2& point, float radAngle) {
	return glm::vec2(point.x * cos(radAngle) - point.y * sin(radAngle), point.x * sin(radAngle) + point.y * cos(radAngle));
}


namespace RE {

Glyph::Glyph(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy texture, int depth, Colour colour) :
	texture(texture),
	depth(depth),
	topLeft({posSize.x, posSize.y + posSize.w}, colour, {uv.x, uv.y + uv.w}),
	topRight({posSize.x + posSize.z, posSize.y + posSize.w}, colour, {uv.x + uv.z, uv.y + uv.w}),
	botLeft({posSize.x, posSize.y}, colour, {uv.x, uv.y}),
	botRight({posSize.x + posSize.z, posSize.y}, colour, {uv.x + uv.z, uv.y}) {

}

Glyph::Glyph(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy texture, int depth, Colour colour, float radAngle, const glm::vec2& origin) :
	texture(texture),
	depth(depth),
	topLeft({glm::vec2{posSize.x, posSize.y} + rotatePoint(glm::vec2(0.0f, posSize.w) - origin, radAngle)}, colour, {uv.x, uv.y + uv.w}),
	topRight({glm::vec2{posSize.x, posSize.y} + rotatePoint(glm::vec2(posSize.z, posSize.w) - origin, radAngle)}, colour, {uv.x + uv.z, uv.y + uv.w}),
	botLeft({glm::vec2{posSize.x, posSize.y} + rotatePoint(glm::vec2(0.0f, 0.0f) - origin, radAngle)}, colour, {uv.x, uv.y}),
	botRight({glm::vec2{posSize.x, posSize.y} + rotatePoint(glm::vec2(posSize.z, 0.0f) - origin, radAngle)}, colour, {uv.x + uv.z, uv.y}) {

}

SpriteBatch::SpriteBatch() {
	GLuint vboBindingPoint = 0u;
	m_vao.setBindingPoint(vboBindingPoint, m_vbo, 0u, sizeof(VertexPOCOUV));

	m_vao.setAttribute(ATTR_POSITION, VertexComponentCount::XY, VertexComponentType::FLOAT, offsetof(VertexPOCOUV, position));
	m_vao.setAttribute(ATTR_COLOUR, VertexComponentCount::RGBA, VertexComponentType::UNSIGNED_BYTE, offsetof(VertexPOCOUV, colour));
	m_vao.setAttribute(ATTR_UV, VertexComponentCount::XY, VertexComponentType::FLOAT, offsetof(VertexPOCOUV, uv));

	m_vao.connectAttributeToBindingPoint(ATTR_POSITION, vboBindingPoint);
	m_vao.connectAttributeToBindingPoint(ATTR_COLOUR, vboBindingPoint);
	m_vao.connectAttributeToBindingPoint(ATTR_UV, vboBindingPoint);
}

SpriteBatch::~SpriteBatch() {

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
void SpriteBatch::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy texture, int depth) {
	m_glyphs.emplace_back(posSize, uv, texture, depth, WHITE);
}
void SpriteBatch::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy texture, int depth, float radAngle, const glm::vec2& origin) {
	m_glyphs.emplace_back(posSize, uv, texture, depth, WHITE, -radAngle, origin);
}
void SpriteBatch::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy texture, int depth, const glm::vec2& direction, const glm::vec2& origin) {
	float radAngle = atan2(direction.y, direction.x);
	m_glyphs.emplace_back(posSize, uv, texture, depth, WHITE, -radAngle, origin);
}
//COLORED
void SpriteBatch::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy texture, int depth, Colour colour) {
	m_glyphs.emplace_back(posSize, uv, texture, depth, colour);
}
void SpriteBatch::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy texture, int depth, Colour colour, float radAngle, const glm::vec2& origin) {
	m_glyphs.emplace_back(posSize, uv, texture, depth, colour, -radAngle, origin);
}
void SpriteBatch::add(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy texture, int depth, Colour colour, const glm::vec2& direction, const glm::vec2& origin) {
	float radAngle = atan2(direction.y, direction.x);
	m_glyphs.emplace_back(posSize, uv, texture, depth, colour, -radAngle, origin);
}
//UNCOLORED, UNSTRETCHED
void SpriteBatch::addTexture(const Texture* texture, const glm::vec2& position, int depth) {
	m_glyphs.emplace_back(glm::vec4(position - texture->getPivot(), texture->getSubimageDims()), m_UVRectangle, TextureProxy{*texture}, depth, WHITE);
}
void SpriteBatch::addTexture(const Texture* texture, const glm::vec2& position, int depth, float radAngle) {
	m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims()), m_UVRectangle, TextureProxy{*texture}, depth, WHITE, radAngle, texture->getPivot());
}
void SpriteBatch::addTexture(const Texture* texture, const glm::vec2& position, int depth, const glm::vec2& direction) {
	float radAngle = atan2(direction.y, direction.x);
	m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims()), m_UVRectangle, TextureProxy{*texture}, depth, WHITE, -radAngle, texture->getPivot());
}
//COLORED, STRETCHED
void SpriteBatch::addTexture(const Texture* texture, const glm::vec2& position, int depth, Colour colour, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
	m_glyphs.emplace_back(glm::vec4(position - texture->getPivot() * scale, texture->getSubimageDims() * scale), m_UVRectangle, TextureProxy{*texture}, depth, colour);
}
void SpriteBatch::addTexture(const Texture* texture, const glm::vec2& position, int depth, Colour colour, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
	m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims() * scale), m_UVRectangle, TextureProxy{*texture}, depth, colour, radAngle, texture->getPivot() * scale);
}
void SpriteBatch::addTexture(const Texture* texture, const glm::vec2& position, int depth, Colour colour, const glm::vec2& direction, const glm::vec2& scale) {
	float radAngle = atan2(direction.y, direction.x);
	m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims() * scale), m_UVRectangle, TextureProxy{*texture}, depth, colour, radAngle, texture->getPivot() * scale);
}
//UNCOLORED, UNSTRETCHED
void SpriteBatch::addSprite(const Sprite& sprite, const glm::vec2& position, int depth) {
	m_glyphs.emplace_back(glm::vec4(position - sprite.getTexture()->getPivot(), sprite.getTexture()->getSubimageDims()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, WHITE);
}
void SpriteBatch::addSprite(const Sprite& sprite, const glm::vec2& position, int depth, float radAngle) {
	m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, WHITE, radAngle, sprite.getTexture()->getPivot());
}
void SpriteBatch::addSprite(const Sprite& sprite, const glm::vec2& position, int depth, const glm::vec2& direction) {
	float radAngle = atan2(direction.y, direction.x);
	m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, WHITE, radAngle, sprite.getTexture()->getPivot());
}
//COLORED, STRETCHED
void SpriteBatch::addSprite(const Sprite& sprite, const glm::vec2& position, int depth, Colour colour, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
	m_glyphs.emplace_back(glm::vec4(position - sprite.getTexture()->getPivot() * scale, sprite.getTexture()->getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, colour);
}
void SpriteBatch::addSprite(const Sprite& sprite, const glm::vec2& position, int depth, Colour colour, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
	m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, colour, radAngle, sprite.getTexture()->getPivot() * scale);
}
void SpriteBatch::addSprite(const Sprite& sprite, const glm::vec2& position, int depth, Colour colour, const glm::vec2& direction, const glm::vec2& scale) {
	float radAngle = atan2(direction.y, direction.x);
	m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, colour, radAngle, sprite.getTexture()->getPivot() * scale);
}
//COLORED, STRETCHED BY FULLSPRITE
void SpriteBatch::addSprite(const FullSprite& sprite, const glm::vec2& position, int depth) {
	m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, sprite.getColour(), 0.0f, sprite.getTexture()->getPivot() * sprite.getScale());
}
void SpriteBatch::addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, float radAngle) {
	m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, sprite.getColour(), radAngle, sprite.getTexture()->getPivot() * sprite.getScale());
}
void SpriteBatch::addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, const glm::vec2& direction) {
	float radAngle = atan2(direction.y, direction.x);
	m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, sprite.getColour(), radAngle, sprite.getTexture()->getPivot() * sprite.getScale());
}
//COLORED, STRETCHED BY USER
void SpriteBatch::addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, Colour colour, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
	m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, colour, 0.0f, sprite.getTexture()->getPivot() * scale);
}
void SpriteBatch::addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, Colour colour, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
	m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, colour, radAngle, sprite.getTexture()->getPivot() * scale);
}
void SpriteBatch::addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, Colour colour, const glm::vec2& direction, const glm::vec2& scale) {
	float radAngle = atan2(direction.y, direction.x);
	m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), TextureProxy{*sprite.getTexture()}, depth, colour, radAngle, sprite.getTexture()->getPivot() * scale);
}
//UNCOLORED, UNSTRETCHED
void SpriteBatch::addSubimage(const Texture* texture, const glm::vec2& position, int depth, const glm::vec2& subImg_Spr) {
	m_glyphs.emplace_back(glm::vec4(position - texture->getPivot(), texture->getSubimageDims()), glm::vec4(glm::floor(subImg_Spr) / texture->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / texture->getSubimagesSpritesCount()), TextureProxy{*texture}, depth, WHITE);
}
void SpriteBatch::addSubimage(const Texture* texture, const glm::vec2& position, int depth, float radAngle, const glm::vec2& subImg_Spr) {
	m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims()), glm::vec4(glm::floor(subImg_Spr) / texture->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / texture->getSubimagesSpritesCount()), TextureProxy{*texture}, depth, WHITE, radAngle, texture->getPivot());
}
void SpriteBatch::addSubimage(const Texture* texture, const glm::vec2& position, int depth, const glm::vec2& direction, const glm::vec2& subImg_Spr) {
	float radAngle = atan2(direction.y, direction.x);
	m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims()), glm::vec4(glm::floor(subImg_Spr) / texture->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / texture->getSubimagesSpritesCount()), TextureProxy{*texture}, depth, WHITE, radAngle, texture->getPivot());
}
//COLORED, STRETCHED
void SpriteBatch::addSubimage(const Texture* texture, const glm::vec2& position, int depth, Colour colour, const glm::vec2& subImg_Spr, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
	m_glyphs.emplace_back(glm::vec4(position - texture->getPivot() * scale, texture->getSubimageDims() * scale), glm::vec4(glm::floor(subImg_Spr) / texture->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / texture->getSubimagesSpritesCount()), TextureProxy{*texture}, depth, colour);
}
void SpriteBatch::addSubimage(const Texture* texture, const glm::vec2& position, int depth, Colour colour, float radAngle, const glm::vec2& subImg_Spr, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
	m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims() * scale), glm::vec4(glm::floor(subImg_Spr) / texture->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / texture->getSubimagesSpritesCount()), TextureProxy{*texture}, depth, colour, radAngle, texture->getPivot() * scale);
}
void SpriteBatch::addSubimage(const Texture* texture, const glm::vec2& position, int depth, Colour colour, const glm::vec2& direction, const glm::vec2& subImg_Spr, const glm::vec2& scale) {
	float radAngle = atan2(direction.y, direction.x);
	m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims() * scale), glm::vec4(glm::floor(subImg_Spr) / texture->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / texture->getSubimagesSpritesCount()), TextureProxy{*texture}, depth, colour, radAngle, texture->getPivot() * scale);
}
//UNCOLORED, UNSTRETCHED
void SpriteBatch::addSurface(const Surface& surface, const glm::vec2& position, int depth, int index) {
	m_glyphs.emplace_back(glm::vec4(position - surface.getPivot(), surface.getDims()), m_SUVRectangle, surface.getTextureProxy(index), depth, WHITE);
}
void SpriteBatch::addSurface(const Surface& surface, const glm::vec2& position, int depth, int index, float radAngle) {
	m_glyphs.emplace_back(glm::vec4(position, surface.getDims()), m_SUVRectangle, surface.getTextureProxy(index), depth, WHITE, radAngle, surface.getPivot());
}
void SpriteBatch::addSurface(const Surface& surface, const glm::vec2& position, int depth, int index, const glm::vec2& direction) {
	float radAngle = atan2(direction.y, direction.x);
	m_glyphs.emplace_back(glm::vec4(position, surface.getDims()), m_SUVRectangle, surface.getTextureProxy(index), depth, WHITE, radAngle, surface.getPivot());
}
//COLORED, STRETCHED
void SpriteBatch::addSurface(const Surface& surface, const glm::vec2& position, int depth, int index, Colour colour, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
	m_glyphs.emplace_back(glm::vec4(position - surface.getPivot() * scale, (glm::vec2)surface.getDims() * scale), m_SUVRectangle, surface.getTextureProxy(index), depth, colour);
}
void SpriteBatch::addSurface(const Surface& surface, const glm::vec2& position, int depth, int index, Colour colour, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
	m_glyphs.emplace_back(glm::vec4(position, (glm::vec2)surface.getDims() * scale), m_SUVRectangle, surface.getTextureProxy(index), depth, colour, radAngle, surface.getPivot() * scale);
}
void SpriteBatch::addSurface(const Surface& surface, const glm::vec2& position, int depth, int index, Colour colour, const glm::vec2& direction, const glm::vec2& scale) {
	float radAngle = atan2(direction.y, direction.x);
	m_glyphs.emplace_back(glm::vec4(position, (glm::vec2)surface.getDims() * scale), m_SUVRectangle, surface.getTextureProxy(index), depth, colour, radAngle, surface.getPivot() * scale);
}

void SpriteBatch::draw() {
	draw(*m_shaderProgram);
}

void SpriteBatch::draw(const ShaderProgram& program) {
	program.use();
	m_vao.bind();

	for (size_t i = 0u; i < m_drawBatches.size(); i++) {
		m_drawBatches[i].texture.bind();
		m_vao.renderArrays(Primitive::TRIANGLES, m_drawBatches[i].offset, m_drawBatches[i].count);
	}

	m_vao.unbind();
	program.unuse();
}

void SpriteBatch::switchShaderProgram(ShaderProgramPtr shaderProgram) {
	m_shaderProgram = shaderProgram;
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

bool SpriteBatch::compareNegToPos(Glyph* a, Glyph* b) {
	return (a->depth > b->depth);
}

bool SpriteBatch::comparePosToNeg(Glyph* a, Glyph* b) {
	return (a->depth < b->depth);
}

bool SpriteBatch::compareTexture(Glyph* a, Glyph* b) {
	return (a->texture > b->texture);
}

}