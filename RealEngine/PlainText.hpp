#pragma once
#include <RealEngine/Button.hpp>

namespace RGUI {

	class PlainText : public Button {
	public:
		PlainText();
		PlainText(const glm::vec2& botLeft, const glm::vec2& dims, const std::string& text, RE::TexturePtr tex, RE::SpriteBatch* spriteBatch, const RE::Font* font, const RE::Colour* textTint, int depth);
		~PlainText();

		void step(const glm::vec2& cursorPos) override;

		void onPress(const glm::vec2& cursorPos) override;
		void onHover(const glm::vec2& cursorPos) override;
		void onRelease(const glm::vec2& cursorPos) override;
	};

}