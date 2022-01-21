#pragma once
#include <vector>
#include <string>

#include <RealEngine/GUI/Button.hpp>

namespace RGUI {

	class ClosedList : public Button {
		template<typename T>friend class Menu;
		friend class MenuParser;
	public:
		ClosedList();
		ClosedList(const glm::vec2& botLeft, const glm::vec2& dims, RE::TexturePtr tex, RE::SpriteBatch* spriteBatch, const RE::Font* font, const RE::Colour* textTint, int depth, const std::vector<RE::FontString>& list);
		~ClosedList();

		void init(const glm::vec2& botLeft, const glm::vec2& dims, RE::TexturePtr tex, RE::SpriteBatch* spriteBatch, const RE::Font* font, const RE::Colour* textTint, int depth, const std::vector<RE::FontString>& list);

		bool overlaps(const glm::vec2& cursorPos) const override;

		void onRelease(const glm::vec2& cursorPos) override;

		RE::FontString getOption() const;
		size_t getChosen() const { return p_chosen; };

		void choose(size_t index);

		void changelist(const std::vector<RE::FontString>& newList);
	protected:
		void disable() override;

		void drawBackground() const override;
		void drawOptionBackground(float index) const;

		void drawText() const override;
		void drawOptionText(size_t index) const;

		size_t p_chosen = 0u;
		std::vector<RE::FontString> p_list;
		bool p_expanded = false;
		float p_optionHeight;
	};

}