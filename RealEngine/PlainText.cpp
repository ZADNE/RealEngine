#include <RealEngine/PlainText.hpp>


namespace RGUI {

	PlainText::PlainText() {

	}

	PlainText::PlainText(const glm::vec2& botLeft, const glm::vec2& dims, const std::string& text, RE::TexturePtr tex, RE::SpriteBatch* spriteBatch, const RE::Font* font, const RE::Colour* textTint, int depth) :
		Button{ botLeft, dims, text, tex, spriteBatch, font, textTint, depth } {

	}


	PlainText::~PlainText() {

	}

	void PlainText::step(const glm::vec2& cursorPos) {

	}

	void PlainText::onPress(const glm::vec2& cursorPos) {

	}

	void PlainText::onHover(const glm::vec2& cursorPos) {

	}

	void PlainText::onRelease(const glm::vec2& cursorPos) {

	}

}