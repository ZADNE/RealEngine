#pragma once
#include <RealEngine/GUI/Button.hpp>

#include <RealEngine/user_input/TypingHandle.hpp>

namespace RGUI {

class TextField : public Button {
	template<typename T>friend class Menu;
	friend class MenuParser;
public:
	//Tex = texture containing 5 subimages - 0 = bottom-left corner of the button, 1 = bottom edge of the button, 2 = filling of the buttom, 3 = bottom 3-side corner used for joining separator and edge, 4 = vertical separator
	//Edge and filling are stretched to desired dimensions
	TextField(const glm::vec2& botLeft, const glm::vec2& dims, const RE::FontString& text, RE::TexturePtr tex, RE::SpriteBatch* spriteBatch, const RE::Font* font, const RE::Colour* textTint, int depth, RE::TypingHandle handle);
	~TextField();

	void init(const glm::vec2& botLeft, const glm::vec2& dims, const RE::FontString& text, RE::TexturePtr tex, RE::SpriteBatch* spriteBatch, const RE::Font* font, const RE::Colour* textTint, int depth, RE::TypingHandle handle);

	RE::FontString* getTextFieldString() {
		return &p_typingHandle.getString();
	};

	virtual void step(const glm::vec2& cursorPos);
	virtual void onGlobalRelease(const glm::vec2& cursorPos);

	void onPress(const glm::vec2& cursorPos) override;
	void onHover(const glm::vec2& cursorPos) override;
	void onRelease(const glm::vec2& cursorPos) override;

protected:
	void disable() override;

	//void drawBackground() const override;
	void drawText() const override;

	bool p_writing = false;

	RE::TypingHandle p_typingHandle;
	size_t p_steps = 0u;
};

}