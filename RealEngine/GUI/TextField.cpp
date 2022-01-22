#include <RealEngine/GUI/TextField.hpp>

#include <RealEngine/main/MainProgram.hpp>
#include <RealEngine/graphics/CursorDrawer.hpp>

namespace RGUI {

TextField::TextField(const glm::vec2& botLeft, const glm::vec2& dims, const RE::FontString& text, RE::TexturePtr tex, RE::SpriteBatch* spriteBatch, const RE::Font* font, const RE::Colour* textTint, int depth, RE::TypingHandle handle) :
	Button{botLeft, dims, text, tex, spriteBatch, font, textTint, depth}, p_typingHandle(handle){

}

TextField::~TextField() {

}

void TextField::init(const glm::vec2& botLeft, const glm::vec2& dims, const RE::FontString& text, RE::TexturePtr tex, RE::SpriteBatch* spriteBatch, const RE::Font* font, const RE::Colour* textTint, int depth, RE::TypingHandle handle) {
	Button::init(botLeft, dims, text, tex, spriteBatch, font, textTint, depth);
	p_typingHandle = handle;
}

void TextField::step(const glm::vec2& cursorPos) {
	if (p_state == STATE_DISABLED) { return; }
	++p_steps;
	if (!p_writing && p_state != STATE_NORMAL) {
		RE::CursorDrawer::setCursor(RE::CURSOR_STATE::ARROW);
		p_state = STATE_NORMAL;
	}
}

void TextField::onGlobalRelease(const glm::vec2& cursorPos) {
	if (p_state == STATE_DISABLED) { return; }
	p_writing = false;
	p_typingHandle.type(false, false);
	p_state = STATE_NORMAL;
}

void TextField::onPress(const glm::vec2& cursorPos) {

}

void TextField::onHover(const glm::vec2& cursorPos) {
	++p_steps;
	if (!p_writing) {
		p_state = STATE_HIGHLIGHTED;
	}
	RE::CursorDrawer::setCursor(RE::CURSOR_STATE::IBEAM);
}

void TextField::onRelease(const glm::vec2& cursorPos) {
	p_writing = !p_writing;
	if (p_writing) {
		p_typingHandle.type(true, false);
		p_state = STATE_USED;
	} else {
		p_typingHandle.type(false, false);
		p_state = STATE_HIGHLIGHTED;
	}
}

void TextField::disable() {
	p_state = STATE_DISABLED;
	p_writing = false;
	p_typingHandle.type(false, false);
}

/*void TextField::drawBackground() const {
	//4 corners
	p_spriteBatch->addSubimage(*p_tex, p_botleft, p_depth, glm::vec2(RGUI::SUBIMAGE_CORNER, p_state));
	p_spriteBatch->addSubimage(*p_tex, p_botleft + glm::vec2(p_dims.x, 0.0f), p_depth, p_white, 1.57079632679f, glm::vec2(RGUI::SUBIMAGE_CORNER, p_state));
	p_spriteBatch->addSubimage(*p_tex, p_botleft + p_dims, p_depth, p_white, 3.14159265359f, glm::vec2(RGUI::SUBIMAGE_CORNER, p_state));
	p_spriteBatch->addSubimage(*p_tex, p_botleft + glm::vec2(0.0f, p_dims.y), p_depth, p_white, 4.71238898038f, glm::vec2(RGUI::SUBIMAGE_CORNER, p_state));
	//6 edges
	p_spriteBatch->addSubimage(*p_tex, p_botleft + glm::vec2(p_cornerDim.x, 0.0f), p_depth, p_white, glm::vec2(RGUI::SUBIMAGE_EDGE, p_state), glm::vec2(p_edgeScale.x, 1.0f));
	p_spriteBatch->addSubimage(*p_tex, p_botleft + glm::vec2(p_cornerDim.x * 0.5f + p_dims.x * 0.5f, 0.0f), p_depth, p_white, glm::vec2(RGUI::SUBIMAGE_EDGE, p_state), glm::vec2(p_edgeScale.x, 1.0f));

	p_spriteBatch->addSubimage(*p_tex, p_botleft + glm::vec2(p_dims.x, p_cornerDim.y), p_depth, p_white, 1.57079632679f, glm::vec2(RGUI::SUBIMAGE_EDGE, p_state), glm::vec2(p_edgeScale.y, 1.0f));

	p_spriteBatch->addSubimage(*p_tex, p_botleft + glm::vec2(p_dims.x - p_cornerDim.x, p_dims.y), p_depth, p_white, 3.14159265359f, glm::vec2(RGUI::SUBIMAGE_EDGE, p_state), glm::vec2(p_edgeScale.x, 1.0f));
	p_spriteBatch->addSubimage(*p_tex, p_botleft + glm::vec2(p_dims.x * 0.5f - p_cornerDim.x * 0.5f, p_dims.y), p_depth, p_white, 3.14159265359f, glm::vec2(RGUI::SUBIMAGE_EDGE, p_state), glm::vec2(p_edgeScale.x, 1.0f));

	p_spriteBatch->addSubimage(*p_tex, p_botleft + glm::vec2(0.0f, p_dims.y - p_cornerDim.y), p_depth, p_white, 4.71238898038f, glm::vec2(RGUI::SUBIMAGE_EDGE, p_state), glm::vec2(p_edgeScale.y, 1.0f));
	//2 fillings
	p_spriteBatch->addSubimage(*p_tex, p_botleft + p_cornerDim, p_depth, p_white, glm::vec2(RGUI::SUBIMAGE_FILLING, p_state), p_edgeScale);
	p_spriteBatch->addSubimage(*p_tex, p_botleft + glm::vec2(p_cornerDim.x * 0.5f + p_dims.x * 0.5f, p_cornerDim.y), p_depth, p_white, glm::vec2(RGUI::SUBIMAGE_FILLING, p_state), p_edgeScale);
	//2 middle-corners
	p_spriteBatch->addSubimage(*p_tex, p_botleft + glm::vec2(p_dims.x * 0.5f - p_cornerDim.x * 0.5f, 0.0f), p_depth, glm::vec2(RGUI::SUBIMAGE_MIDDLE_CORNER, p_state));
	p_spriteBatch->addSubimage(*p_tex, p_botleft + glm::vec2(p_dims.x * 0.5f + p_cornerDim.x * 0.5f, p_dims.y), p_depth, p_white, 3.14159265359f, glm::vec2(RGUI::SUBIMAGE_MIDDLE_CORNER, p_state));
	//1 separator
	p_spriteBatch->addSubimage(*p_tex, p_botleft + glm::vec2(p_dims.x * 0.5f - p_cornerDim.x * 0.5f, p_cornerDim.y), p_depth, p_white, glm::vec2(RGUI::SUBIMAGE_SEPARATOR, p_state), glm::vec2(1.0f, p_edgeScale.y));
}*/

/*void TextField::drawText() const {
	//Text
	p_spriteFont->add(*p_spriteBatch, p_text.c_str(), glm::vec2(p_botleft.x + p_cornerDim.x, p_middlePos.y), p_depth, p_textTint[(size_t)p_state], RE::HAlign::RIGHT, RE::VAlign::WHOLE_MIDDLE);
	//Text field
	if (p_writing && ((p_steps / 22u) % 2u) == 0u) {
		p_spriteFont->add(*p_spriteBatch, (p_textField + "|").c_str(), glm::vec2(p_middlePos.x + p_cornerDim.x, p_middlePos.y), p_depth, p_textTint[(size_t)p_state], RE::HAlign::RIGHT, RE::VAlign::WHOLE_MIDDLE);
	} else {
		p_spriteFont->add(*p_spriteBatch, p_textField.c_str(), glm::vec2(p_middlePos.x + p_cornerDim.x, p_middlePos.y), p_depth, p_textTint[(size_t)p_state], RE::HAlign::RIGHT, RE::VAlign::WHOLE_MIDDLE);
	}
}*/

void TextField::drawText() const {
	RE::FontString str = p_typingHandle.visit();
	if (!p_writing && str.empty()) {
		//Default text
		p_font->add(*p_spriteBatch, p_text, p_middlePos, p_depth + 1, p_textTint[(size_t)p_state], RE::HAlign::MIDDLE, RE::VAlign::WHOLE_MIDDLE);

	} else if (p_writing && ((p_steps / 22u) % 2u) == 0u) {
		//Text field
		p_font->add(*p_spriteBatch, str + '|', p_middlePos, p_depth + 1, p_textTint[(size_t)p_state], RE::HAlign::MIDDLE, RE::VAlign::WHOLE_MIDDLE);
	} else {
		p_font->add(*p_spriteBatch, str + ' ', p_middlePos, p_depth + 1, p_textTint[(size_t)p_state], RE::HAlign::MIDDLE, RE::VAlign::WHOLE_MIDDLE);
	}
}

}