#pragma once
#include <RealEngine/Button.hpp>

namespace RGUI {

	class Slider : public Button {
	public:
		Slider();
		Slider(const glm::vec2& botLeft, const glm::vec2& dims, const std::string& text, RE::TexturePtr tex, RE::SpriteBatch* spriteBatch, const RE::Font* font, const RE::Colour* textTint, int depth, float minValue = 0.0f, float maxValue = 1.0f, float steps = 101.0f, unsigned char decimals = 2u);
		~Slider();

		void init(const glm::vec2& botLeft, const glm::vec2& dims, const std::string& text, RE::TexturePtr tex, RE::SpriteBatch* spriteBatch, const RE::Font* font, const RE::Colour* textTint, int depth, float minValue = 0.0f, float maxValue = 1.0f, float steps = 101.0f, unsigned char decimals = 2u);

		void interfere(const glm::vec2& cursorPos);

		void step(const glm::vec2& cursorPos) override;

		void onPress(const glm::vec2& cursorPos) override;
		void onHover(const glm::vec2& cursorPos) override;
		void onRelease(const glm::vec2& cursorPos) override;
		void onGlobalRelease(const glm::vec2& cursorPos) override;

		float getValue() const;
		float getPortion() const;

		void setValue(float value);
		void setPortion(float portion);
	protected:
		void drawBackground() const override;
		void drawText() const override;

		void update();
		void updateText();
		void updateNumbers();

		float p_proportion;
		float p_minValue;
		float p_difference;//Difference between minimal and maximal value
		float p_steps;
		unsigned char p_decimals;

		float p_leftScale;
		float p_leftX;//Offset from p_botleft only

		float p_rightScale;
		float p_rightX;//Offset from p_botleft only
	};

}