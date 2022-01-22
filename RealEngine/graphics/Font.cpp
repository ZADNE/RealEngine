#include <RealEngine/graphics/Font.hpp>

#include <iostream>

#include <RealEngine/main/Error.hpp>
#include <RealEngine/graphics/SpriteBatch.hpp>


int closestPow2(int i) {
	i--;
	int pi = 1;
	while (i > 0) {
		i >>= 1;
		pi <<= 1;
	}
	return pi;
}

#define MAX_TEXTURE_RES 4096

namespace RE {

	Font::Font(const char* font, int size, char32_t cs, char32_t ce) {
		init(font, size, cs, ce);
	}

	void Font::init(const char* font, int size) {
		init(font, size, FIRST_PRINTABLE_CHAR, LAST_PRINTABLE_CHAR);
	}

	void Font::init(const char* font, int size, char32_t cs, char32_t ce) {
		TTF_Font* f = TTF_OpenFont(font, size);
		if (f == nullptr) {
			std::string error = "Failed to open TTF font: ";
			error += font;
			RE::fatalError(error);
		}
		m_cs = cs;
		m_ce = ce;
		m_fontHeight = (float)TTF_FontHeight(f);
		m_fontDescent = (float)TTF_FontDescent(f);
		m_fontAscent = (float)TTF_FontAscent(f);
		m_regStart = cs;
		m_regLength = ce - cs + 1;
		int padding = size / 8;

		// First measure all the regions
		glm::ivec4* glyphRects = new glm::ivec4[m_regLength];
		int i = 0, advance;
		for (unsigned int c = cs; c <= ce; c++) {
			TTF_GlyphMetrics(f, c, &glyphRects[i].x, &glyphRects[i].z, &glyphRects[i].y, &glyphRects[i].w, &advance);
			glyphRects[i].z = advance;
			glyphRects[i].x = 0;
			//glyphRects[i].w -= glyphRects[i].y;
			glyphRects[i].y = 0;
			i++;
		}

		// Find best partitioning of glyphs
		int rows = 1, w, h, bestWidth = 0, bestHeight = 0, area = MAX_TEXTURE_RES * MAX_TEXTURE_RES, bestRows = 0;
		std::vector<int>* bestPartition = nullptr;
		while (rows <= m_regLength) {
			h = rows * (padding + (int)m_fontHeight) + padding;
			auto gr = createRows(glyphRects, m_regLength, rows, padding, w);

			// Desire a power of 2 texture
			w = closestPow2(w);
			h = closestPow2(h);

			// A texture must be feasible
			if (w > MAX_TEXTURE_RES || h > MAX_TEXTURE_RES) {
				rows++;
				delete[] gr;
				continue;
			}

			// Check for minimal area
			if (area >= w * h) {
				if (bestPartition) delete[] bestPartition;
				bestPartition = gr;
				bestWidth = w;
				bestHeight = h;
				bestRows = rows;
				area = bestWidth * bestHeight;
				rows++;
			}
			else {
				delete[] gr;
				break;
			}
		}

		// Can a bitmap font be made?
		if (!bestPartition) {
			fprintf(stderr, "Failed to Map TTF font %s to texture. Try lowering resolution.\n", font);
			fflush(stderr);
			throw 282;
		}
		// Create the texture
		glGenTextures(1, &m_texID);
		glBindTexture(GL_TEXTURE_2D, m_texID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bestWidth, bestHeight, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

		// Now draw all the glyphs
		SDL_Color fg = { 255, 255, 255, 255 };
		int ly = padding;
		for (int ri = 0; ri < bestRows; ri++) {
			int lx = padding;
			for (size_t ci = 0; ci < bestPartition[ri].size(); ci++) {
				int gi = bestPartition[ri][ci];

				SDL_Surface* glyphSurface = TTF_RenderGlyph_Blended(f, cs + gi, fg);


				// Pre-multiplication occurs here
				unsigned char* sp = (unsigned char*)glyphSurface->pixels;
				size_t cp = glyphSurface->w * glyphSurface->h * 4;
				int counter = -1;
				//Turning the RGBA texture into RED only texture (using the beginning of the texture for that)
				for (size_t i = 0u; i < cp; i += 4u) {
					sp[++counter] = sp[i + 3];
				}

				// Save glyph image and update coordinates
				glTexSubImage2D(GL_TEXTURE_2D, 0, lx, bestHeight - ly - 1 - glyphSurface->h, glyphSurface->w, glyphSurface->h, GL_RED, GL_UNSIGNED_BYTE, glyphSurface->pixels);

				glyphRects[gi].x = lx;
				glyphRects[gi].y = ly;
				glyphRects[gi].z = glyphSurface->w;
				glyphRects[gi].w = glyphSurface->h;

				SDL_FreeSurface(glyphSurface);
				glyphSurface = nullptr;

				lx += glyphRects[gi].z + padding;
			}
			ly += (int)m_fontHeight + padding;
		}


		// Draw the unsupported glyph
		int rs = padding - 1;
		/*int* blackTransparency = new int[rs * rs];
		memset(blackTransparency, 0, rs * rs * sizeof(int));
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rs, rs, GL_RG, GL_UNSIGNED_BYTE, blackTransparency);
		delete[] blackTransparency;
		blackTransparency = nullptr;*/

		//Texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//Swizzling
		GLint swizzles[4u] = { GL_ONE, GL_ONE, GL_ONE, GL_RED };
		glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzles);

		//Create spriteBatch glyphs
		m_glyphs = new CharGlyph[m_regLength + 1];
		for (i = 0; i < m_regLength; i++) {
			m_glyphs[i].character = (char)(cs + i);
			//std::cout << m_glyphs[i].character << std::endl;
			m_glyphs[i].size = glm::vec2(glyphRects[i].z, glyphRects[i].w);
			m_glyphs[i].uvRect = glm::vec4(
				(float)glyphRects[i].x / (float)bestWidth,
				(float)glyphRects[i].y / (float)bestHeight,
				(float)glyphRects[i].z / (float)bestWidth,
				(float)glyphRects[i].w / (float)bestHeight
			);
		}
		m_glyphs[m_regLength].character = ' ';
		m_glyphs[m_regLength].size = m_glyphs[0].size;
		m_glyphs[m_regLength].uvRect = glm::vec4(0, 0, (float)rs / (float)bestWidth, (float)rs / (float)bestHeight);

		glBindTexture(GL_TEXTURE_2D, 0);
		delete[] glyphRects;
		delete[] bestPartition;
		TTF_CloseFont(f);
	}

	void Font::dispose() {
		if (m_texID != 0) {
			glDeleteTextures(1, &m_texID);
			m_texID = 0;
		}
		if (m_glyphs) {
			delete[] m_glyphs;
			m_glyphs = nullptr;
		}
	}

	std::vector<int>* Font::createRows(glm::ivec4* rects, int rectsLength, int r, int padding, int& w) {
		// Blank initialize
		std::vector<int>* l = new std::vector<int>[r]();
		int* cw = new int[r]();
		for (int i = 0; i < r; i++) {
			cw[i] = 2 * padding;//Padding on left and right side
		}

		// Loop through all glyphs
		for (int i = 0; i < rectsLength; i++) {
			// Find row for placement
			int ri = 0;
			for (int rii = 1; rii < r; rii++)
				if (cw[rii] < cw[ri]) ri = rii;

			// Add width to that row
			cw[ri] += rects[i].z + padding;

			// Add glyph to the row list
			l[ri].push_back(i);
		}

		// Find the max width
		w = 0;
		for (int i = 0; i < r; i++) {
			if (cw[i] > w) w = cw[i];
		}

		return l;
	}

	glm::vec2 Font::measure(const FontString& s) const {
		glm::vec2 size(0, m_fontHeight);
		float cw = 0;
		for (const auto& c: s) {
			if (c == '\n') {
				size.y += m_fontHeight;
				if (size.x < cw)
					size.x = cw;
				cw = 0;
			}else {
				int gi = c - m_regStart;
				if (gi < 0 || gi >= m_regLength)
					gi = m_regLength;
				cw += m_glyphs[gi].size.x;
			}
		}
		if (size.x < cw)
			size.x = cw;
		return size;
	}

	glm::vec2 Font::measure(const FontString& s, const glm::vec2& rots) const {
		glm::vec2 size(0, m_fontHeight);
		float cw = 0;
		for (const auto& c: s) {
			if (c == '\n') {
				size.y += m_fontHeight;
				if (size.x < cw) {
					size.x = cw;
				}
				cw = 0;
			}
			else {
				int gi = c - m_regStart;
				if (gi < 0 || gi >= m_regLength)
					gi = m_regLength;
				cw += m_glyphs[gi].size.x;
			}
		}
		if (size.x < cw)
			size.x = cw;
		return size;
	}

	//UNSCALED, UNROTATED
	void Font::add(SpriteBatch& batch, const FontString& s, const glm::vec2& position, int depth, Colour tint,
		HAlign halign/* = HAlign::RIGHT*/, VAlign valign/* = VAlign::FIRST_LINE_ABOVE*/) const {
		glm::vec2 tp = position;
		tp.y += m_fontDescent;
		//Apply justification
		glm::vec2 textDims = measure(s);

		int numberOfLines;

		switch (valign) {
		case VAlign::FIRST_LINE_ABOVE:

			break;
		case VAlign::WHOLE_ABOVE:
			tp.y += textDims.y - m_fontHeight;
			break;
		case VAlign::FIRST_LINE_MIDDLE:
			tp.y += -m_fontHeight / 2.0f - m_fontDescent;
			break;
		case VAlign::WHOLE_MIDDLE:
			numberOfLines = (int)(textDims.y / m_fontHeight);
			tp.y += ((float)numberOfLines / 2.0f - 1.0f) * m_fontHeight - m_fontDescent;
			break;
		case VAlign::BELOW:
			tp.y -= m_fontAscent;
			break;
		}

		glm::vec2 trueOrigin = tp;

		auto temp = s;

		switch (halign) {
		case HAlign::RIGHT:
			tp.x = trueOrigin.x;
			break;
		case HAlign::LEFT:
			temp = s.substr(0u, s.find_first_of("\n", 0u));
			tp.x = trueOrigin.x - measure(temp).x;
			break;
		case HAlign::MIDDLE:
			temp = s.substr(0u, s.find_first_of("\n", 0u));
			tp.x = trueOrigin.x - measure(temp).x / 2.0f;
			break;
		}
		for (int si = 0; s[si] != 0; si++) {
			auto c = s[si];
			if (s[si] == '\n') {
				tp.y -= m_fontHeight;
				switch (halign) {
				case HAlign::RIGHT:
					tp.x = trueOrigin.x;
					break;
				case HAlign::LEFT:
					temp = s.substr(si + 1u, s.find_first_of("\n", si + 1u) - 1u - si);
					tp.x = trueOrigin.x - measure(temp).x;
					break;
				case HAlign::MIDDLE:
					temp = s.substr(si + 1u, s.find_first_of("\n", si + 1u) - 1u - si);
					tp.x = trueOrigin.x - measure(temp).x / 2.0f;
					break;
				}
			} else {
				int gi = c - m_regStart;
				if (gi < 0 || gi >= m_regLength) gi = m_regLength;
				glm::vec4 destRect(tp, m_glyphs[gi].size);
				batch.add(destRect, m_glyphs[gi].uvRect, m_texID, depth, tint);
				tp.x += m_glyphs[gi].size.x;
			}
		}
	}

	//SCALED, UNROTATED
	void Font::add(SpriteBatch& batch, const FontString& s, const glm::vec2& position, const glm::vec2& scaling,
		int depth, Colour tint, HAlign halign/* = HAlign::RIGHT*/, VAlign valign/* = VAlign::FIRST_LINE_ABOVE*/) const {
		glm::vec2 tp = position;
		tp.y += m_fontDescent * scaling.y;
		//Apply justification
		glm::vec2 textDims = measure(s) * scaling;

		int numberOfLines;
		switch (valign) {
		case VAlign::FIRST_LINE_ABOVE:

			break;
		case VAlign::WHOLE_ABOVE:
			tp.y += textDims.y - m_fontHeight * scaling.y;
			break;
		case VAlign::FIRST_LINE_MIDDLE:
			tp.y -= m_fontAscent * scaling.y / 2.0f;
			break;
		case VAlign::WHOLE_MIDDLE:
			numberOfLines = (int)(textDims.y / (m_fontHeight * scaling.y));
			tp.y += ((float)numberOfLines / 2.0f - 1.0f) * m_fontHeight * scaling.y - m_fontDescent * scaling.y;
			break;
		case VAlign::BELOW:
			tp.y -= m_fontAscent * scaling.y;
			break;
		}

		glm::vec2 trueOrigin = tp;

		auto temp = s;

		switch (halign) {
		case HAlign::RIGHT:
			tp.x = trueOrigin.x;
			break;
		case HAlign::LEFT:
			temp = s.substr(0u, s.find_first_of("\n", 0u));
			tp.x = trueOrigin.x - measure(temp).x * scaling.x;
			break;
		case HAlign::MIDDLE:
			temp = s.substr(0u, s.find_first_of("\n", 0u));
			tp.x = trueOrigin.x - measure(temp).x / 2.0f * scaling.x;
			break;
		}

		for (int si = 0; s[si] != 0; si++) {
			auto c = s[si];
			if (s[si] == L'\n') {
				tp.y -= m_fontHeight * scaling.y;
				switch (halign) {
				case HAlign::RIGHT:
					tp.x = trueOrigin.x;
					break;
				case HAlign::LEFT:
					temp = s.substr(si + 1u, s.find_first_of("\n", si + 1u) - 1u - si);
					tp.x = trueOrigin.x - measure(temp).x * scaling.x;
					break;
				case HAlign::MIDDLE:
					temp = s.substr(si + 1u, s.find_first_of("\n", si + 1u) - 1u - si);
					tp.x = trueOrigin.x - measure(temp).x / 2.0f * scaling.x;
					break;
				}
			}
			else {
				// Check for correct glyph
				int gi = c - m_regStart;
				if (gi < 0 || gi >= m_regLength) gi = m_regLength;
				glm::vec4 destRect(tp, m_glyphs[gi].size * scaling);
				batch.add(destRect, m_glyphs[gi].uvRect, m_texID, depth, tint);
				tp.x += m_glyphs[gi].size.x * scaling.x;
			}
		}
	}

	//UNSCALED, ROTATED
	void Font::add(SpriteBatch& batch, const FontString& s, const glm::vec2& position, int depth, Colour tint,
		const glm::vec2& rots, HAlign halign/* = HAlign::RIGHT*/, VAlign valign/* = VAlign::FIRST_LINE_ABOVE*/) const {
		glm::vec2 tp = position;
		tp.y += m_fontDescent;
		//Apply justification
		glm::vec2 textDims = measure(s);

		int numberOfLines;

		switch (valign) {
		case VAlign::FIRST_LINE_ABOVE:

			break;
		case VAlign::WHOLE_ABOVE:
			tp.y += textDims.y - m_fontHeight;
			break;
		case VAlign::FIRST_LINE_MIDDLE:
			tp.y += -m_fontHeight / 2.0f - m_fontDescent;
			break;
		case VAlign::WHOLE_MIDDLE:
			numberOfLines = (int)(textDims.y / m_fontHeight);
			tp.y += ((float)numberOfLines / 2.0f - 1.0f) * m_fontHeight - m_fontDescent;
			break;
		case VAlign::BELOW:
			tp.y -= m_fontAscent;
			break;
		}

		glm::vec2 trueOrigin = tp;

		auto temp = s;

		switch (halign) {
		case HAlign::RIGHT:
			tp.x = trueOrigin.x;
			break;
		case HAlign::LEFT:
			temp = s.substr(0u, s.find_first_of("\n", 0u));
			tp.x = trueOrigin.x - measure(temp).x;
			break;
		case HAlign::MIDDLE:
			temp = s.substr(0u, s.find_first_of("\n", 0u));
			tp.x = trueOrigin.x - measure(temp).x / 2.0f;
			break;
		}
		float lineIndex = 0.0f;
		for (int si = 0; s[si] != 0; si++) {
			auto c = s[si];
			if (s[si] == L'\n') {
				switch (halign) {
				case HAlign::RIGHT:
					tp.x = trueOrigin.x + m_fontHeight * (++lineIndex) * sin(rots.x);
					tp.y = trueOrigin.y - m_fontHeight * lineIndex * cos(rots.x);
					break;
				case HAlign::LEFT:
					temp = s.substr(si + 1u, s.find_first_of("\n", si + 1u) - 1u - si);
					tp.x = trueOrigin.x - measure(temp).x * cos(rots.x) + (m_fontHeight * ++lineIndex) * sin(rots.x);
					tp.y = trueOrigin.y - (m_fontHeight * lineIndex) * cos(rots.x) - measure(temp).x * sin(rots.x);
					break;
				case HAlign::MIDDLE:
					temp = s.substr(si + 1u, s.find_first_of("\n", si + 1u) - 1u - si);
					tp.x = trueOrigin.x - measure(temp).x / 2.0f;
					break;
				}
			} else {
				int gi = c - m_regStart;
				if (gi < 0 || gi >= m_regLength) { gi = m_regLength; }
				glm::vec4 destRect(tp, m_glyphs[gi].size);
				batch.add(destRect, m_glyphs[gi].uvRect, m_texID, depth, tint);
				tp.x += m_glyphs[gi].size.x * cos(rots.x);
				tp.y += m_glyphs[gi].size.y * sin(rots.x);
			}
		}
	}

	//SCALED, ROTATED
	void Font::add(SpriteBatch& batch, const FontString& s, const glm::vec2& position, const glm::vec2& scaling,
		int depth, Colour tint, const glm::vec2& rots, HAlign halign/* = HAlign::RIGHT*/, VAlign valign/* = VAlign::FIRST_LINE_ABOVE*/) const {

	}

}