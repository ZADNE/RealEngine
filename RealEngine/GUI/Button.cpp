#include <RealEngine/GUI/Button.hpp>

#include <RealEngine/graphics/CursorDrawer.hpp>

namespace RGUI {

	Button::Button() {

	}

	Button::Button(const glm::vec2& botLeft, const glm::vec2& dims, const RE::FontString& text, RE::TexturePtr tex, RE::SpriteBatch* spriteBatch, const RE::Font* font, const RE::Colour* textTint, int depth) :
		p_botleft(botLeft),
		p_dims(dims),
		p_text(text),
		p_tex(tex),
		p_spriteBatch(spriteBatch),
		p_font(font),
		p_depth(depth),
		p_edgeScale((p_dims - 2.0f * p_cornerDim) / p_cornerDim),
		p_middlePos(p_botleft + p_dims * 0.5f),
		p_textTint{ textTint[0], textTint[1], textTint[2], textTint[3] },
		p_cornerDim(p_tex->getSubimageDims()){

	}


	Button::~Button() {

	}

	void Button::init(const glm::vec2& botLeft, const glm::vec2& dims, const RE::FontString& text, RE::TexturePtr tex, RE::SpriteBatch* spriteBatch, const RE::Font* font, const RE::Colour* textTint, int depth) {
		p_botleft = botLeft;
		p_dims = dims;
		p_text = text;
		p_tex = tex;
		p_spriteBatch = spriteBatch;
		p_font = font;
		p_depth = depth;
		p_cornerDim = p_tex->getSubimageDims();
		p_edgeScale = (p_dims - 2.0f * p_cornerDim) / p_cornerDim;
		p_middlePos = p_botleft + p_dims * 0.5f;
		p_textTint[0] = textTint[0];
		p_textTint[1] = textTint[1];
		p_textTint[2] = textTint[2];
		p_textTint[3] = textTint[3];
	}

	bool Button::overlaps(const glm::vec2& cursorPos) const {
		if (p_botleft.x <= cursorPos.x && (p_botleft.x + p_dims.x) >= cursorPos.x &&
			p_botleft.y <= cursorPos.y && (p_botleft.y + p_dims.y) >= cursorPos.y) {
			return true;
		}
		return false;
	}

	void Button::onGlobalPress(const glm::vec2& cursorPos) {

	}

	void Button::step(const glm::vec2& cursorPos) {
		if (p_state == STATE_DISABLED) { return; }
		if (p_state != STATE_NORMAL) {
			RE::CursorDrawer::setCursor(RE::CURSOR_STATE::ARROW);
		}
		p_state = STATE_NORMAL;
	}

	void Button::onGlobalRelease(const glm::vec2& cursorPos) {

	}

	bool Button::press(const glm::vec2& cursorPos) {
		bool returnVal = overlaps(cursorPos);
		if (returnVal && p_state != STATE_DISABLED) {
			onPress(cursorPos);
			return true;
		}
		return false;
	}

	void Button::onPress(const glm::vec2& cursorPos) {
		p_state = STATE_USED;
	}

	bool Button::hover(const glm::vec2& cursorPos) {
		bool returnVal = overlaps(cursorPos);
		if (returnVal && p_state != STATE_DISABLED) {
			onHover(cursorPos);
			return true;
		}
		return false;
	}

	void Button::onHover(const glm::vec2& cursorPos) {
		if (p_state != STATE_USED) {
			p_state = STATE_HIGHLIGHTED;
		}
		RE::CursorDrawer::setCursor(RE::CURSOR_STATE::HAND);
	}

	bool Button::release(const glm::vec2& cursorPos) {
		bool returnVal = overlaps(cursorPos);
		if (returnVal && p_state != STATE_DISABLED) {
			onRelease(cursorPos);
			return true;
		}
		return false;
	}

	void Button::onRelease(const glm::vec2& cursorPos) {
		p_state = STATE_HIGHLIGHTED;
	}

	void Button::enable() {
		p_state = STATE_NORMAL;
	}

	void Button::disable() {
		p_state = STATE_DISABLED;
	}

	void Button::draw() const {
		drawBackground();
		drawText();
	}

	void Button::drawBackground() const {
		//4 corners
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft, p_depth, glm::vec2(RGUI::SUBIMAGE_CORNER, p_state));
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_dims.x, 0.0f), p_depth, p_white, 1.57079632679f, glm::vec2(RGUI::SUBIMAGE_CORNER, p_state));
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft + p_dims, p_depth, p_white, 3.14159265359f, glm::vec2(RGUI::SUBIMAGE_CORNER, p_state));
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(0.0f, p_dims.y), p_depth, p_white, 4.71238898038f, glm::vec2(RGUI::SUBIMAGE_CORNER, p_state));
		//4 edges
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_cornerDim.x, 0.0f), p_depth, p_white, glm::vec2(RGUI::SUBIMAGE_EDGE, p_state), glm::vec2(p_edgeScale.x, 1.0f));
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_dims.x, p_cornerDim.y), p_depth, p_white, 1.57079632679f, glm::vec2(RGUI::SUBIMAGE_EDGE, p_state), glm::vec2(p_edgeScale.y, 1.0f));
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_dims.x - p_cornerDim.x, p_dims.y), p_depth, p_white, 3.14159265359f, glm::vec2(RGUI::SUBIMAGE_EDGE, p_state), glm::vec2(p_edgeScale.x, 1.0f));
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(0.0f, p_dims.y - p_cornerDim.y), p_depth, p_white, 4.71238898038f, glm::vec2(RGUI::SUBIMAGE_EDGE, p_state), glm::vec2(p_edgeScale.y, 1.0f));
		//Filling
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft + p_cornerDim, p_depth, p_white, glm::vec2(RGUI::SUBIMAGE_FILLING, p_state), p_edgeScale);
	}

	void Button::drawText() const {
		//Text
		p_font->add(*p_spriteBatch, p_text, p_middlePos, p_depth, p_textTint[(size_t)p_state], RE::HAlign::MIDDLE, RE::VAlign::WHOLE_MIDDLE);
	}

}