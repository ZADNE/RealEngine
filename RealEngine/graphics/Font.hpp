#pragma once
#include <vector>
#include <string>

#include <SDL2/SDL_ttf.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <RealEngine/graphics/Vertex.hpp>

namespace RE {
	class SpriteBatch;

	using FontChar = char;
	using FontString = std::basic_string<FontChar>;
	using FontStringStream = std::basic_stringstream<FontChar>;

	struct CharGlyph {
	public:
		FontChar character;
		glm::vec4 uvRect;
		glm::vec2 size;
	};

#define FIRST_PRINTABLE_CHAR (32)
#define LAST_PRINTABLE_CHAR (126)

	//For text justification
	enum class HAlign {
		LEFT, MIDDLE, RIGHT
	};
	enum class VAlign {
		WHOLE_ABOVE, FIRST_LINE_ABOVE, FIRST_LINE_MIDDLE, WHOLE_MIDDLE, BELOW
	};

	class Font {
	public:
		Font() {}
		Font(const char* font, int size, char32_t cs, char32_t ce);
		Font(const char* font, int size) :
			Font(font, size, FIRST_PRINTABLE_CHAR, LAST_PRINTABLE_CHAR) {
		}

		void init(const char* font, int size);
		void init(const char* font, int size, char32_t cs, char32_t ce);

		//Destroys the font resources
		void dispose();

		float getFontHeight() const { return m_fontHeight; }
		char32_t getStartChar() const { return m_cs; }
		char32_t getEndChar() const { return m_ce; }

		//Measures the dimensions of the text
		glm::vec2 measure(const FontString& s) const;
		glm::vec2 measure(const FontString& s, const glm::vec2& rots) const;


		//UNSCALED, UNROTATED
		void add(SpriteBatch& batch, const FontString& s, const glm::vec2& position,
			int depth, Colour tint, HAlign halign = HAlign::RIGHT, VAlign valign = VAlign::FIRST_LINE_ABOVE) const;

		//SCALED, UNROTATED
		void add(SpriteBatch& batch, const FontString& s, const glm::vec2& position, const glm::vec2& scaling,
			int depth, Colour tint, HAlign halign = HAlign::RIGHT, VAlign valign = VAlign::FIRST_LINE_ABOVE) const;

		//UNSCALED, ROTATED
		void add(SpriteBatch& batch, const FontString& s, const glm::vec2& position,
			int depth, Colour tint, const glm::vec2& rots, HAlign halign = HAlign::RIGHT, VAlign valign = VAlign::FIRST_LINE_ABOVE) const;

		//SCALED, ROTATED
		void add(SpriteBatch& batch, const FontString& s, const glm::vec2& position, const glm::vec2& scaling,
			int depth, Colour tint, const glm::vec2& rots, HAlign halign = HAlign::RIGHT, VAlign valign = VAlign::FIRST_LINE_ABOVE) const;


		//Unsafe
		unsigned int getTextureID() const {
			return m_texID;
		}
	private:
		static std::vector<int>* createRows(glm::ivec4* rects, int rectsLength, int r, int padding, int& w);

		int m_regStart, m_regLength;
		CharGlyph* m_glyphs = nullptr;
		float m_fontHeight;
		float m_fontDescent;
		float m_fontAscent;

		unsigned int m_texID = 0u;

		char32_t m_cs = FIRST_PRINTABLE_CHAR;
		char32_t m_ce = LAST_PRINTABLE_CHAR;
	};
}
