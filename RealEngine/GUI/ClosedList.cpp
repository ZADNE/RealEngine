#include <RealEngine/GUI/ClosedList.hpp>

#include <algorithm>

namespace RGUI {

	ClosedList::ClosedList() {

	}

	ClosedList::ClosedList(const glm::vec2& botLeft, const glm::vec2& dims, RE::TexturePtr tex, RE::SpriteBatch* spriteBatch, const RE::Font* font, const RE::Colour* textTint, int depth, const std::vector<RE::FontString>& list) :
		Button{ botLeft, dims, list[0], tex, spriteBatch, font, textTint, depth },
		p_list(list),
		p_optionHeight(p_dims.y - p_cornerDim.y){

	}

	ClosedList::~ClosedList() {

	}

	void ClosedList::init(const glm::vec2& botLeft, const glm::vec2& dims, RE::TexturePtr tex, RE::SpriteBatch* spriteBatch, const RE::Font* font, const RE::Colour* textTint, int depth, const std::vector<RE::FontString>& list) {
		Button::init(botLeft, dims, p_list[p_chosen], tex, spriteBatch, font, textTint, depth);
		p_list = list;
	}

	bool ClosedList::overlaps(const glm::vec2& cursorPos) const {
		if (!p_expanded) {
			return Button::overlaps(cursorPos);
		}
		else {
			if (p_botleft.x <= cursorPos.x && (p_botleft.x + p_dims.x) >= cursorPos.x &&
				(p_botleft.y - (float)p_list.size() * p_optionHeight) <= cursorPos.y && (p_botleft.y + p_dims.y) >= cursorPos.y) {
				return true;
			}
			return false;
		}
	}

	void ClosedList::onRelease(const glm::vec2& cursorPos) {
		if (p_expanded) {
			int helper1 = (int)(p_botleft.y + (int)p_optionHeight - cursorPos.y) - 1;
			int helper2 = (int)p_optionHeight;
			size_t helperF = helper1 / helper2;
			if (helperF) {
				p_chosen = helperF - 1u;
			}
			p_text = p_list[p_chosen];
			p_expanded = false;
		}
		else {
			p_expanded = true;
		}
		p_state = STATE_HIGHLIGHTED;
	}

	RE::FontString ClosedList::getOption() const {
		return p_list[p_chosen];
	}

	void ClosedList::choose(size_t index) {
		p_chosen = index;
		p_chosen = std::min(p_chosen, p_list.size() - 1);
		p_text = p_list[p_chosen];
	}

	void ClosedList::changelist(const std::vector<RE::FontString>& newList) {
		p_list = newList;
		p_chosen = std::min(p_chosen, p_list.size() - 1);
		p_text = p_list[p_chosen];
	}

	void ClosedList::disable() {
		p_state = STATE_DISABLED;
		p_expanded = false;
	}

	void ClosedList::drawBackground() const {
		if (!p_expanded) {
			Button::drawBackground();
		}
		else {
			for (float i = 0.0f; i <= (float)p_list.size(); ++i) {
				drawOptionBackground(i);
			}
		}
	}

	void ClosedList::drawOptionBackground(float index) const {
		if (index != 0.0f) {//Middle option
			//2 middle-corners
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_dims.x, -p_optionHeight * (-1.0f + index)), p_depth + 1, p_white, 1.57079632679f, glm::vec2(RGUI::SUBIMAGE_MIDDLE_CORNER, p_state));
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(0.0f, -p_optionHeight * (-1.0f + index) + p_cornerDim.y), p_depth + 1, p_white, 4.71238898038f, glm::vec2(RGUI::SUBIMAGE_MIDDLE_CORNER, p_state));
			//Top separator
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_cornerDim.x, -p_optionHeight * (-1.0f + index)), p_depth + 1, p_white, glm::vec2(RGUI::SUBIMAGE_SEPARATOR, p_state), glm::vec2(p_edgeScale.x, 1.0f));
		}
		else {//First option
		 //2 corners
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_dims.x, -p_optionHeight * (-1.0f + index) + p_cornerDim.y), p_depth + 1, p_white, 3.14159265359f, glm::vec2(RGUI::SUBIMAGE_CORNER, p_state));
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(0.0f, -p_optionHeight * (-1.0f + index) + p_cornerDim.y), p_depth + 1, p_white, 4.71238898038f, glm::vec2(RGUI::SUBIMAGE_CORNER, p_state));
			//Top edge
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_cornerDim.x, -p_optionHeight * (-1.0f + index)), p_depth + 1, p_white, glm::vec2(RGUI::SUBIMAGE_EDGE, p_state), glm::vec2(p_edgeScale.x, 1.0f));
		}

		if ((size_t)index >= p_list.size() - 1u) {//Last option
			//2 corners
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(0.0f, -p_optionHeight * index), p_depth + 1, glm::vec2(RGUI::SUBIMAGE_CORNER, p_state));
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_dims.x, -p_optionHeight * index), p_depth + 1, p_white, 1.57079632679f, glm::vec2(RGUI::SUBIMAGE_CORNER, p_state));
			//Bottom edge
			p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_cornerDim.x, -p_optionHeight * index), p_depth + 1, p_white, 0.0f, glm::vec2(RGUI::SUBIMAGE_EDGE, p_state), glm::vec2(p_edgeScale.x, 1.0f));
		}
		//2 side edges
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(p_dims.x, p_cornerDim.y + -p_optionHeight * index), p_depth + 1, p_white, 1.57079632679f, glm::vec2(RGUI::SUBIMAGE_EDGE, p_state), glm::vec2(p_edgeScale.y, 1.0f));
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft + glm::vec2(0.0f, -p_optionHeight * (-1.0f + index)), p_depth + 1, p_white, 4.71238898038f, glm::vec2(RGUI::SUBIMAGE_EDGE, p_state), glm::vec2(p_edgeScale.y, 1.0f));
		//Filling
		p_spriteBatch->addSubimage(p_tex.get(), p_botleft + p_cornerDim + glm::vec2(0.0f, -p_optionHeight * index), p_depth + 1, p_white, glm::vec2(RGUI::SUBIMAGE_FILLING, p_state), p_edgeScale);
	}

	void ClosedList::drawText() const {
		if (!p_expanded) {
			Button::drawText();
		}
		else {
			for (size_t i = 0u; i <= p_list.size(); ++i) {
				drawOptionText(i);
			}
		}
	}

	void ClosedList::drawOptionText(size_t index) const {
		if (index != 0u) {
			p_font->add(*p_spriteBatch, p_list[index - 1u], glm::vec2(p_middlePos.x, p_middlePos.y - (float)index * p_optionHeight), p_depth + 2, p_textTint[(size_t)p_state], RE::HAlign::MIDDLE, RE::VAlign::WHOLE_MIDDLE);
		}
		else {
			p_font->add(*p_spriteBatch, p_list[p_chosen], p_middlePos, p_depth + 2, p_textTint[(size_t)p_state], RE::HAlign::MIDDLE, RE::VAlign::WHOLE_MIDDLE);
		}
	}

}