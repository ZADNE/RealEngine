#include <RealEngine/SpriteBatch.hpp>
#include <RealEngine/ResourceManager.hpp>

#include <algorithm>


namespace RE {

	Glyph::Glyph(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, GLuint Texture, int Depth, Colour colour) {
		texture = Texture;
		depth = Depth;
		topLeft.colour = colour;
		topRight.colour = colour;
		botLeft.colour = colour;
		botRight.colour = colour;

		topLeft.setPosition(posSizeRectangle.x, posSizeRectangle.y + posSizeRectangle.w);
		topRight.setPosition(posSizeRectangle.x + posSizeRectangle.z, posSizeRectangle.y + posSizeRectangle.w);
		botLeft.setPosition(posSizeRectangle.x, posSizeRectangle.y);
		botRight.setPosition(posSizeRectangle.x + posSizeRectangle.z, posSizeRectangle.y);

		topLeft.setUV(uvRectagle.x, uvRectagle.y + uvRectagle.w);
		topRight.setUV(uvRectagle.x + uvRectagle.z, uvRectagle.y + uvRectagle.w);
		botLeft.setUV(uvRectagle.x, uvRectagle.y);
		botRight.setUV(uvRectagle.x + uvRectagle.z, uvRectagle.y);
	}

	Glyph::Glyph(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, GLuint Texture, int Depth, Colour colour, float radAngle, const glm::vec2& origin) {
		texture = Texture;
		depth = Depth;
		topLeft.colour = colour;
		topRight.colour = colour;
		botLeft.colour = colour;
		botRight.colour = colour;

		//Unrotated
		glm::vec2 tL = glm::vec2(0.0f, posSizeRectangle.w) - origin;
		glm::vec2 tR = glm::vec2(posSizeRectangle.z, posSizeRectangle.w) - origin;
		glm::vec2 bL = glm::vec2(0.0f, 0.0f) - origin;
		glm::vec2 bR = glm::vec2(posSizeRectangle.z, 0.0f) - origin;
		//Rotating
		tL = rotatePoint(tL, radAngle);
		tR = rotatePoint(tR, radAngle);
		bL = rotatePoint(bL, radAngle);
		bR = rotatePoint(bR, radAngle);


		topLeft.setPosition(posSizeRectangle.x + tL.x, posSizeRectangle.y + tL.y);
		topRight.setPosition(posSizeRectangle.x + tR.x, posSizeRectangle.y + tR.y);
		botLeft.setPosition(posSizeRectangle.x + bL.x, posSizeRectangle.y + bL.y);
		botRight.setPosition(posSizeRectangle.x + bR.x, posSizeRectangle.y + bR.y);

		topLeft.setUV(uvRectagle.x, uvRectagle.y + uvRectagle.w);
		topRight.setUV(uvRectagle.x + uvRectagle.z, uvRectagle.y + uvRectagle.w);
		botLeft.setUV(uvRectagle.x, uvRectagle.y);
		botRight.setUV(uvRectagle.x + uvRectagle.z, uvRectagle.y);
	}

	glm::vec2 Glyph::rotatePoint(const glm::vec2& point, float radAngle) const {
		glm::vec2 rotated = point;
		rotated.x = point.x * cos(radAngle) - point.y * sin(radAngle);
		rotated.y = point.x * sin(radAngle) + point.y * cos(radAngle);
		return rotated;
	}

	SpriteBatch::SpriteBatch() {
		createVertexArray();
	}

	SpriteBatch::~SpriteBatch() {
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(1, &m_vbo);
	}

	void SpriteBatch::begin(GlyphSortType sortType) {
		m_glyphSortType = sortType;
		m_drawBatches.clear();
		m_glyphs.clear();
	}

	void SpriteBatch::end() {
		m_glyphPointers.resize(m_glyphs.size());
		for (size_t i = 0; i < m_glyphs.size(); i++) {
			m_glyphPointers[i] = &m_glyphs[i];
		}
		sortGlyphs();
		createDrawBatches();
	}

	//UNCOLORED
	void SpriteBatch::add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, GLuint texture, int depth) {
		m_glyphs.emplace_back(posSizeRectangle, uvRectagle, texture, depth, m_whiteColour);
	}
	void SpriteBatch::add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, GLuint texture, int depth, float radAngle, const glm::vec2& origin) {
		m_glyphs.emplace_back(posSizeRectangle, uvRectagle, texture, depth, m_whiteColour, -radAngle, origin);
	}
	void SpriteBatch::add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, GLuint texture, int depth, const glm::vec2& direction, const glm::vec2& origin) {
		float radAngle = atan2(direction.y, direction.x);
		m_glyphs.emplace_back(posSizeRectangle, uvRectagle, texture, depth, m_whiteColour, -radAngle, origin);
	}
	//COLORED
	void SpriteBatch::add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, GLuint texture, int depth, Colour colour) {
		m_glyphs.emplace_back(posSizeRectangle, uvRectagle, texture, depth, colour);
	}
	void SpriteBatch::add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, GLuint texture, int depth, Colour colour, float radAngle, const glm::vec2& origin) {
		m_glyphs.emplace_back(posSizeRectangle, uvRectagle, texture, depth, colour, -radAngle, origin);
	}
	void SpriteBatch::add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, GLuint texture, int depth, Colour colour, const glm::vec2& direction, const glm::vec2& origin) {
		float radAngle = atan2(direction.y, direction.x);
		m_glyphs.emplace_back(posSizeRectangle, uvRectagle, texture, depth, colour, -radAngle, origin);
	}
	//UNCOLORED, UNSTRETCHED
	void SpriteBatch::addTexture(const Texture* texture, const glm::vec2& position, int depth) {
		m_glyphs.emplace_back(glm::vec4(position - texture->getPivot(), texture->getSubimageDims()), m_UVRectangle, texture->getID(), depth, m_whiteColour);
	}
	void SpriteBatch::addTexture(const Texture* texture, const glm::vec2& position, int depth, float radAngle) {
		m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims()), m_UVRectangle, texture->getID(), depth, m_whiteColour, radAngle, texture->getPivot());
	}
	void SpriteBatch::addTexture(const Texture* texture, const glm::vec2& position, int depth, const glm::vec2& direction) {
		float radAngle = atan2(direction.y, direction.x);
		m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims()), m_UVRectangle, texture->getID(), depth, m_whiteColour, -radAngle, texture->getPivot());
	}
	//COLORED, STRETCHED
	void SpriteBatch::addTexture(const Texture* texture, const glm::vec2& position, int depth, Colour colour, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
		m_glyphs.emplace_back(glm::vec4(position - texture->getPivot() * scale, texture->getSubimageDims() * scale), m_UVRectangle, texture->getID(), depth, colour);
	}
	void SpriteBatch::addTexture(const Texture* texture, const glm::vec2& position, int depth, Colour colour, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
		m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims() * scale), m_UVRectangle, texture->getID(), depth, colour, radAngle, texture->getPivot() * scale);
	}
	void SpriteBatch::addTexture(const Texture* texture, const glm::vec2& position, int depth, Colour colour, const glm::vec2& direction, const glm::vec2& scale) {
		float radAngle = atan2(direction.y, direction.x);
		m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims() * scale), m_UVRectangle, texture->getID(), depth, colour, radAngle, texture->getPivot() * scale);
	}
	//UNCOLORED, UNSTRETCHED
	void SpriteBatch::addSprite(const Sprite& sprite, const glm::vec2& position, int depth) {
		m_glyphs.emplace_back(glm::vec4(position - sprite.getTexture()->getPivot(), sprite.getTexture()->getSubimageDims()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), sprite.getTexture()->getID(), depth, m_whiteColour);
	}
	void SpriteBatch::addSprite(const Sprite& sprite, const glm::vec2& position, int depth, float radAngle) {
		m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), sprite.getTexture()->getID(), depth, m_whiteColour, radAngle, sprite.getTexture()->getPivot());
	}
	void SpriteBatch::addSprite(const Sprite& sprite, const glm::vec2& position, int depth, const glm::vec2& direction) {
		float radAngle = atan2(direction.y, direction.x);
		m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), sprite.getTexture()->getID(), depth, m_whiteColour, radAngle, sprite.getTexture()->getPivot());
	}
	//COLORED, STRETCHED
	void SpriteBatch::addSprite(const Sprite& sprite, const glm::vec2& position, int depth, Colour colour, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
		m_glyphs.emplace_back(glm::vec4(position - sprite.getTexture()->getPivot() * scale, sprite.getTexture()->getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), sprite.getTexture()->getID(), depth, colour);
	}
	void SpriteBatch::addSprite(const Sprite& sprite, const glm::vec2& position, int depth, Colour colour, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
		m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), sprite.getTexture()->getID(), depth, colour, radAngle, sprite.getTexture()->getPivot() * scale);
	}
	void SpriteBatch::addSprite(const Sprite& sprite, const glm::vec2& position, int depth, Colour colour, const glm::vec2& direction, const glm::vec2& scale) {
		float radAngle = atan2(direction.y, direction.x);
		m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), sprite.getTexture()->getID(), depth, colour, radAngle, sprite.getTexture()->getPivot() * scale);
	}
	//COLORED, STRETCHED BY FULLSPRITE
	void SpriteBatch::addSprite(const FullSprite& sprite, const glm::vec2& position, int depth) {
		m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), sprite.getTexture()->getID(), depth, sprite.getColour(), 0.0f, sprite.getTexture()->getPivot() * sprite.getScale());
	}
	void SpriteBatch::addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, float radAngle) {
		m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), sprite.getTexture()->getID(), depth, sprite.getColour(), radAngle, sprite.getTexture()->getPivot() * sprite.getScale());
	}
	void SpriteBatch::addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, const glm::vec2& direction) {
		float radAngle = atan2(direction.y, direction.x);
		m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * sprite.getScale()), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), sprite.getTexture()->getID(), depth, sprite.getColour(), radAngle, sprite.getTexture()->getPivot() * sprite.getScale());
	}
	//COLORED, STRETCHED BY USER
	void SpriteBatch::addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, Colour colour, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
		m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), sprite.getTexture()->getID(), depth, colour, 0.0f, sprite.getTexture()->getPivot() * scale);
	}
	void SpriteBatch::addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, Colour colour, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
		m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), sprite.getTexture()->getID(), depth, colour, radAngle, sprite.getTexture()->getPivot() * scale);
	}
	void SpriteBatch::addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, Colour colour, const glm::vec2& direction, const glm::vec2& scale) {
		float radAngle = atan2(direction.y, direction.x);
		m_glyphs.emplace_back(glm::vec4(position, sprite.getTexture()->getSubimageDims() * scale), glm::vec4(glm::floor(sprite.getSubimageSprite()) / sprite.getTexture()->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / sprite.getTexture()->getSubimagesSpritesCount()), sprite.getTexture()->getID(), depth, colour, radAngle, sprite.getTexture()->getPivot() * scale);
	}
	//UNCOLORED, UNSTRETCHED
	void SpriteBatch::addSubimage(const Texture* texture, const glm::vec2& position, int depth, const glm::vec2& subImg_Spr) {
		m_glyphs.emplace_back(glm::vec4(position - texture->getPivot(), texture->getSubimageDims()), glm::vec4(glm::floor(subImg_Spr) / texture->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / texture->getSubimagesSpritesCount()), texture->getID(), depth, m_whiteColour);
	}
	void SpriteBatch::addSubimage(const Texture* texture, const glm::vec2& position, int depth, float radAngle, const glm::vec2& subImg_Spr) {
		m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims()), glm::vec4(glm::floor(subImg_Spr) / texture->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / texture->getSubimagesSpritesCount()), texture->getID(), depth, m_whiteColour, radAngle, texture->getPivot());
	}
	void SpriteBatch::addSubimage(const Texture* texture, const glm::vec2& position, int depth, const glm::vec2& direction, const glm::vec2& subImg_Spr) {
		float radAngle = atan2(direction.y, direction.x);
		m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims()), glm::vec4(glm::floor(subImg_Spr) / texture->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / texture->getSubimagesSpritesCount()), texture->getID(), depth, m_whiteColour, radAngle, texture->getPivot());
	}
	//COLORED, STRETCHED
	void SpriteBatch::addSubimage(const Texture* texture, const glm::vec2& position, int depth, Colour colour, const glm::vec2& subImg_Spr, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
		m_glyphs.emplace_back(glm::vec4(position - texture->getPivot() * scale, texture->getSubimageDims() * scale), glm::vec4(glm::floor(subImg_Spr) / texture->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / texture->getSubimagesSpritesCount()), texture->getID(), depth, colour);
	}
	void SpriteBatch::addSubimage(const Texture* texture, const glm::vec2& position, int depth, Colour colour, float radAngle, const glm::vec2& subImg_Spr, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
		m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims() * scale), glm::vec4(glm::floor(subImg_Spr) / texture->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / texture->getSubimagesSpritesCount()), texture->getID(), depth, colour, radAngle, texture->getPivot() * scale);
	}
	void SpriteBatch::addSubimage(const Texture* texture, const glm::vec2& position, int depth, Colour colour, const glm::vec2& direction, const glm::vec2& subImg_Spr, const glm::vec2& scale) {
		float radAngle = atan2(direction.y, direction.x);
		m_glyphs.emplace_back(glm::vec4(position, texture->getSubimageDims() * scale), glm::vec4(glm::floor(subImg_Spr) / texture->getSubimagesSpritesCount(), glm::vec2(1.0f, 1.0f) / texture->getSubimagesSpritesCount()), texture->getID(), depth, colour, radAngle, texture->getPivot() * scale);
	}
	//UNCOLORED, UNSTRETCHED
	void SpriteBatch::addSurface(const Surface& surface, const glm::vec2& position, int depth, int index) {
		m_glyphs.emplace_back(glm::vec4(position - surface.getPivot(), surface.getDims()), m_SUVRectangle, surface.getTextureID(index), depth, m_whiteColour);
	}
	void SpriteBatch::addSurface(const Surface& surface, const glm::vec2& position, int depth, int index, float radAngle) {
		m_glyphs.emplace_back(glm::vec4(position, surface.getDims()), m_SUVRectangle, surface.getTextureID(index), depth, m_whiteColour, radAngle, surface.getPivot());
	}
	void SpriteBatch::addSurface(const Surface& surface, const glm::vec2& position, int depth, int index, const glm::vec2& direction) {
		float radAngle = atan2(direction.y, direction.x);
		m_glyphs.emplace_back(glm::vec4(position, surface.getDims()), m_SUVRectangle, surface.getTextureID(index), depth, m_whiteColour, radAngle, surface.getPivot());
	}
	//COLORED, STRETCHED
	void SpriteBatch::addSurface(const Surface& surface, const glm::vec2& position, int depth, int index, Colour colour, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
		m_glyphs.emplace_back(glm::vec4(position - surface.getPivot() * scale, (glm::vec2)surface.getDims() * scale), m_SUVRectangle, surface.getTextureID(index), depth, colour);
	}
	void SpriteBatch::addSurface(const Surface& surface, const glm::vec2& position, int depth, int index, Colour colour, float radAngle, const glm::vec2& scale/* = glm::vec2(1.0f, 1.0f)*/) {
		m_glyphs.emplace_back(glm::vec4(position, (glm::vec2)surface.getDims() * scale), m_SUVRectangle, surface.getTextureID(index), depth, colour, radAngle, surface.getPivot() * scale);
	}
	void SpriteBatch::addSurface(const Surface& surface, const glm::vec2& position, int depth, int index, Colour colour, const glm::vec2& direction, const glm::vec2& scale) {
		float radAngle = atan2(direction.y, direction.x);
		m_glyphs.emplace_back(glm::vec4(position, (glm::vec2)surface.getDims() * scale), m_SUVRectangle, surface.getTextureID(index), depth, colour, radAngle, surface.getPivot() * scale);
	}

	void SpriteBatch::draw() {
		m_shader->setShader();
		glBindVertexArray(m_vao);

		for (size_t i = 0u; i < m_drawBatches.size(); i++) {
			glBindTexture(GL_TEXTURE_2D, m_drawBatches[i].m_texture);
			glDrawArrays(GL_TRIANGLES, m_drawBatches[i].m_offset, m_drawBatches[i].m_numberVertices);
		}

		glBindVertexArray(0);
		m_shader->resetShader();
	}

	void SpriteBatch::draw(const ShaderProgram& program) {
		program.setShader();
		glBindVertexArray(m_vao);

		for (size_t i = 0u; i < m_drawBatches.size(); i++) {
			glBindTexture(GL_TEXTURE_2D, m_drawBatches[i].m_texture);
			glDrawArrays(GL_TRIANGLES, m_drawBatches[i].m_offset, m_drawBatches[i].m_numberVertices);
		}

		glBindVertexArray(0);
		program.resetShader();
	}

	void SpriteBatch::changeShader(ShaderProgramSeed ss/* = ShaderProgramSeed::stdSprite*/) {
		m_shader = RE::RM::getShaderProgram(ss);
	}

	void SpriteBatch::createVertexArray() {
		if (m_vao == 0) {//If doesn't exist already
			glGenVertexArrays(1, &m_vao);
		}
		glBindVertexArray(m_vao);

		if (m_vbo == 0) {//If doesn't exist already
			glGenBuffers(1, &m_vbo);
		}
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		glEnableVertexAttribArray(ATTLOC_PO);//Position
		glVertexAttribPointer(ATTLOC_PO, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPOCOUV), (void*)offsetof(VertexPOCOUV, position));
		glEnableVertexAttribArray(ATTLOC_CO);//Colour
		glVertexAttribPointer(ATTLOC_CO, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VertexPOCOUV), (void*)offsetof(VertexPOCOUV, colour));
		glEnableVertexAttribArray(ATTLOC_UV);//UV
		glVertexAttribPointer(ATTLOC_UV, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPOCOUV), (void*)offsetof(VertexPOCOUV, uv));

		glBindVertexArray(0);
	}

	void SpriteBatch::sortGlyphs() {
		switch (m_glyphSortType) {
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
		std::vector<VertexPOCOUV> vertices;
		vertices.resize(m_glyphPointers.size() * 6u);

		int currentVertex = 0;
		int offset = 0;

		m_drawBatches.emplace_back(offset, 6, m_glyphPointers[0]->texture);
		vertices[currentVertex++] = m_glyphPointers[0]->topLeft;
		vertices[currentVertex++] = m_glyphPointers[0]->botLeft;
		vertices[currentVertex++] = m_glyphPointers[0]->topRight;
		vertices[currentVertex++] = m_glyphPointers[0]->topRight;
		vertices[currentVertex++] = m_glyphPointers[0]->botLeft;
		vertices[currentVertex++] = m_glyphPointers[0]->botRight;

		offset += 6;

		for (size_t i = 1; i < m_glyphPointers.size(); i++) {
			if (m_glyphPointers[i]->texture != m_glyphPointers[i - 1]->texture) {
				m_drawBatches.emplace_back(offset, 6, m_glyphPointers[i]->texture);
			}
			else {
				m_drawBatches.back().m_numberVertices += 6;
			}
			vertices[currentVertex++] = m_glyphPointers[i]->topLeft;
			vertices[currentVertex++] = m_glyphPointers[i]->botLeft;
			vertices[currentVertex++] = m_glyphPointers[i]->topRight;
			vertices[currentVertex++] = m_glyphPointers[i]->topRight;
			vertices[currentVertex++] = m_glyphPointers[i]->botLeft;
			vertices[currentVertex++] = m_glyphPointers[i]->botRight;
			offset += 6;
		}

		//Uploading to VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexPOCOUV), NULL, GL_DYNAMIC_DRAW);//Oprhaning the buffer
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(VertexPOCOUV), vertices.data());//Uploading
		glBindBuffer(GL_ARRAY_BUFFER, 0);
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