#pragma once
#include <string>

#include <RealEngine/Controller.hpp>
#include <RealEngine/Texture.hpp>
#include <RealEngine/SpriteBatch.hpp>
#include <RealEngine/Font.hpp>

namespace RGUI {

	class Button : public Controller {
		template<typename T>friend class Menu;
		friend class MenuParser;
	public:
		//Tex = texture containing 3 subimages - 0 = bottom-left corner of the button, 1 = bottom edge of the button, 2 = filling of the buttom
		//Edge and filling are stretched to desired dimensions
		//The subimages should not have odd dimensions and also should have origin of [0; 0]
		Button();
		Button(const glm::vec2& botLeft, const glm::vec2& dims, const RE::FontString& text, RE::TexturePtr tex, RE::SpriteBatch* spriteBatch, const RE::Font* font, const RE::Colour* textTint, int depth);
		~Button();

		virtual void init(const glm::vec2& botLeft, const glm::vec2& dims, const RE::FontString& text, RE::TexturePtr tex, RE::SpriteBatch* spriteBatch, const RE::Font* font, const RE::Colour* textTint, int depth);

		//Returns whether the cursor does point on the button
		bool overlaps(const glm::vec2& cursorPos) const override;


		virtual void onGlobalPress(const glm::vec2& cursorPos);

		//Should be called every physics step
		virtual void step(const glm::vec2& cursorPos);

		virtual void onGlobalRelease(const glm::vec2& cursorPos);

		//Returns whether onPress() was called (successfully pressed the button)
		bool press(const glm::vec2& cursorPos);
		virtual void onPress(const glm::vec2& cursorPos);
		//Returns whether onHover() was called (successfully hovering over the button)
		bool hover(const glm::vec2& cursorPos);
		virtual void onHover(const glm::vec2& cursorPos);
		//Returns whether onRelease() was called (successfully released on the button)
		bool release(const glm::vec2& cursorPos);
		virtual void onRelease(const glm::vec2& cursorPos);


		//Should be called each draw step
		virtual void draw() const;

		RE::FontString getText() const {
			return p_text;
		};

		RE::FontString& getText() {
			return p_text;
		};

	protected:
		virtual void enable();
		virtual void disable();

		virtual void drawBackground() const;
		virtual void drawText() const;

		RE::SpriteBatch* p_spriteBatch = nullptr;
		const RE::Font* p_font = nullptr;
		RE::TexturePtr p_tex = nullptr;
		RE::FontString p_text;
		glm::vec2 p_botleft;
		glm::vec2 p_dims;
		glm::vec2 p_cornerDim;
		int p_depth;
		RE::Colour p_white = RE::Colour{ 255, 255, 255, 255 };
		glm::vec2 p_edgeScale;
		glm::vec2 p_middlePos;
		RE::Colour p_textTint[4u];
	};

}