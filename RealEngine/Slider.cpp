#include <RealEngine/Slider.hpp>

#include <iomanip>
#include <sstream>

#include <RealEngine/utility.hpp>
#include <RealEngine/CursorDrawer.hpp>


namespace RGUI {

	Slider::Slider() {

	}

	Slider::Slider(const glm::vec2& botLeft, const glm::vec2& dims, const std::string& text, RE::TexturePtr tex, RE::SpriteBatch* spriteBatch, const RE::Font* font, const RE::Colour* textTint, int depth, float minValue/* = 0.0f*/, float maxValue/* = 1.0f*/, float steps/* = 101.0f*/, unsigned char decimals/* = 2u*/) :
		Button{ botLeft, dims, text, tex, spriteBatch, font, textTint, depth } {
		p_minValue = minValue;
		p_difference = maxValue - minValue;
		p_decimals = decimals;
		p_steps = steps - 1.0f;
		p_proportion = std::round(0.5f * p_steps) / p_steps;
		update();
	}


	Slider::~Slider() {

	}

	void Slider::init(const glm::vec2& botLeft, const glm::vec2& dims, const std::string& text, RE::TexturePtr tex, RE::SpriteBatch* spriteBatch, const RE::Font* font, const RE::Colour* textTint, int depth, float minValue/* = 0.0f*/, float maxValue/* = 1.0f*/, float steps/* = 101.0f*/, unsigned char decimals/* = 2u*/) {
		Button::init(botLeft, dims, text, tex, spriteBatch, font, textTint, depth);
		p_minValue = minValue;
		p_difference = maxValue - minValue;
		p_decimals = decimals;
		p_steps = steps - 1.0f;
		p_proportion = std::round(0.5f * p_steps) / p_steps;
		update();
	}

	void Slider::interfere(const glm::vec2& cursorPos) {
		p_proportion = (cursorPos.x - p_botleft.x - p_dims.x * 0.05f) / (p_dims.x * 0.9f);
		p_proportion = std::round(p_proportion * p_steps) / p_steps;
		p_proportion = rmath::clamp(p_proportion, 0.0f, 1.0f);
		update();
	}

	void Slider::step(const glm::vec2& cursorPos) {
		if (p_state == STATE_USED) {
			interfere(cursorPos);
		}
		else {
			if (p_state == STATE_HIGHLIGHTED) {
				RE::CursorDrawer::setCursor(RE::CURSOR_STATE::ARROW);
			}
			p_state = STATE_NORMAL;
		}
	}

	void Slider::onPress(const glm::vec2& cursorPos) {
		p_state = STATE_USED;
	}

	void Slider::onHover(const glm::vec2& cursorPos) {
		if (p_state == STATE_USED) {
			interfere(cursorPos);
		}
		else {
			p_state = STATE_HIGHLIGHTED;
		}

		RE::CursorDrawer::setCursor(RE::CURSOR_STATE::SIZEWE);
	}

	void Slider::onRelease(const glm::vec2& cursorPos) {
		p_state = STATE_HIGHLIGHTED;
	}

	void Slider::onGlobalRelease(const glm::vec2& cursorPos) {
		if (p_state == STATE_USED) {
			RE::CursorDrawer::setCursor(RE::CURSOR_STATE::ARROW);
		}
		p_state = STATE_NORMAL;
	}

	float Slider::getValue() const {
		return p_minValue + p_proportion * p_difference;
	}

	float Slider::getPortion() const {
		return p_proportion;
	}

	void Slider::setValue(float value) {
		p_proportion = rmath::clamp((value - p_minValue) / p_difference, 0.0f, 1.0f);
		update();
	}

	void Slider::setPortion(float portion) {
		p_proportion = portion;
		update();
	}

	void Slider::drawBackground() const {
		if (p_proportion != 0.0f) {//If the slider isn't completely to the left
			//1 filling
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + p_cornerDim, p_depth, p_white, glm::vec2(SUBIMAGE_FILLING, p_state), glm::vec2(p_leftScale, p_edgeScale.y));
			//2 edges
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_cornerDim.x, 0.0f), p_depth, p_white, glm::vec2(SUBIMAGE_EDGE, p_state), glm::vec2(p_leftScale, 1.0f));
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_leftX, p_dims.y), p_depth, p_white, 3.14159265359f, glm::vec2(SUBIMAGE_EDGE, p_state), glm::vec2(p_leftScale, 1.0f));
		}
		if (p_proportion != 1.0f) {//If the slider isn't completely to the right
			//1 filling
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_rightX, p_cornerDim.y), p_depth, p_white, glm::vec2(SUBIMAGE_FILLING, p_state), glm::vec2(p_rightScale, p_edgeScale.y));
			//2 edges
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_rightX, 0.0f), p_depth, p_white, glm::vec2(SUBIMAGE_EDGE, p_state), glm::vec2(p_rightScale, 1.0f));
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_dims.x - p_cornerDim.x, p_dims.y), p_depth, p_white, 3.14159265359f, glm::vec2(SUBIMAGE_EDGE, p_state), glm::vec2(p_rightScale, 1.0f));
		}

		//4 corners
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft, p_depth, glm::vec2(SUBIMAGE_CORNER, p_state));
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_dims.x, 0.0f), p_depth, p_white, 1.57079632679f, glm::vec2(SUBIMAGE_CORNER, p_state));
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft + p_dims, p_depth, p_white, 3.14159265359f, glm::vec2(SUBIMAGE_CORNER, p_state));
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(0.0f, p_dims.y), p_depth, p_white, 4.71238898038f, glm::vec2(SUBIMAGE_CORNER, p_state));

		//2 edges
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_dims.x, p_cornerDim.y), p_depth, p_white, 1.57079632679f, glm::vec2(SUBIMAGE_EDGE, p_state), glm::vec2(p_edgeScale.y, 1.0f));
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(0.0f, p_dims.y - p_cornerDim.y), p_depth, p_white, 4.71238898038f, glm::vec2(SUBIMAGE_EDGE, p_state), glm::vec2(p_edgeScale.y, 1.0f));
	}

	void Slider::drawText() const {
		//1 filling
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_leftX + p_cornerDim.x, p_cornerDim.y), p_depth, p_white, glm::vec2(SUBIMAGE_FILLING, p_state), glm::vec2(p_dims.x * 0.1f * 0.90909f / p_cornerDim.x - 2.0f, p_edgeScale.y));
		//2 edges
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_leftX + p_cornerDim.x, 0.0f), p_depth, p_white, glm::vec2(SUBIMAGE_EDGE, p_state), glm::vec2(p_dims.x * 0.1f * 0.90909f / p_cornerDim.x - 2.0f, 1.0f));
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_rightX - p_cornerDim.x, p_dims.y), p_depth, p_white, 3.14159265359f, glm::vec2(SUBIMAGE_EDGE, p_state), glm::vec2(p_dims.x * 0.1f * 0.90909f / p_cornerDim.x - 2.0f, 1.0f));

		if (p_proportion != 0.0f) {//If the slider isn't completely to the left
			//2 middle-corners
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_leftX, 0.0f), p_depth, glm::vec2(SUBIMAGE_MIDDLE_CORNER, p_state));
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_leftX + p_cornerDim.x, p_dims.y), p_depth, p_white, 3.14159265359f, glm::vec2(SUBIMAGE_MIDDLE_CORNER, p_state));
			//1 separator
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_leftX + p_cornerDim.x, p_dims.y - p_cornerDim.y), p_depth, p_white, 3.14159265359f, glm::vec2(SUBIMAGE_SEPARATOR, p_state), glm::vec2(1.0f, p_edgeScale.y));
		}
		if (p_proportion != 1.0f) {//If the slider isn't completely to the right
			//2 middle-corners
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_rightX - p_cornerDim.x, 0.0f), p_depth, glm::vec2(SUBIMAGE_MIDDLE_CORNER, p_state));
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_rightX, p_dims.y), p_depth, p_white, 3.14159265359f, glm::vec2(SUBIMAGE_MIDDLE_CORNER, p_state));
			//1 separator
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_rightX - p_cornerDim.x, p_cornerDim.y), p_depth, p_white, 0.0f, glm::vec2(SUBIMAGE_SEPARATOR, p_state), glm::vec2(1.0f, p_edgeScale.y));
		}
		//Text
		p_font->add(*p_spriteBatch, p_text.c_str(), glm::vec2(p_botleft.x + (p_proportion + 0.05f) * p_dims.x * 0.90909f, p_middlePos.y), p_depth + 1, p_textTint[(size_t)p_state], RE::HAlign::MIDDLE, RE::VAlign::WHOLE_MIDDLE);
	}

	void Slider::update() {
		updateText();
		updateNumbers();
	}

	void Slider::updateText() {
		std::stringstream stream;
		stream << std::fixed << std::setprecision(p_decimals) << (p_minValue + p_proportion * p_difference);
		p_text = stream.str();
	}

	void Slider::updateNumbers() {
		p_leftX = p_proportion * p_dims.x * 0.90909f;
		p_rightX = (p_proportion + 0.1f) * p_dims.x * 0.90909f;
		p_leftScale = (p_leftX - p_cornerDim.x) / p_cornerDim.x;
		p_rightScale = (p_dims.x - p_rightX - p_cornerDim.x) / p_cornerDim.x;
	}

}