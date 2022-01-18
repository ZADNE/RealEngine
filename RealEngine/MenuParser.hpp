#pragma once
#include <utility>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <fstream>

#include <glm/vec2.hpp>

#include <RealEngine/Vertex.hpp>

#include <RealEngine/Button.hpp>
#include <RealEngine/TextField.hpp>
#include <RealEngine/Slider.hpp>
#include <RealEngine/PlainText.hpp>
#include <RealEngine/ClosedList.hpp>


#ifdef LM_EXPRESSION_LIST
#define MENU_LM_DEC , RE::LanguageManager LM
#define MENU_LM_PASS , LM
#include <RealEngine/LanguageManager.hpp>
#else
#define MENU_LM_DEC
#define MENU_LM_PASS
#endif // LM_EXPRESSION_LIST

namespace RGUI {

#undef ERROR

	enum class ControllerType {
		TEMPLATE = -2,
		ERROR = -1,
		BUTTON,
		TEXT_FIELD,
		SLIDER,
		PLAIN_TEXT,
		CLOSED_LIST
	};

	enum class MenuParserError {
		OK = 0,

		ERROR_READING_FILE,

		NONEXISTENT_TEMPLATE,

		//COMMON
		COMMON_NO_TYPE,
		COMMON_BAD_TYPE,

		COMMON_NO_POS,
		COMMON_BAD_POS,

		COMMON_NO_DIM,
		COMMON_BAD_DIM,

		COMMON_NO_TEXT,
		COMMON_BAD_MANAGED_TEXT,

		COMMON_NO_TEXTURE_NAME,

		COMMON_NO_FONT_NAME,
		COMMON_NO_FONT_SIZE,
		COMMON_BAD_FONT_SIZE,
		COMMON_NO_FONT_BOUNDS,
		COMMON_BAD_FONT_BOUNDS,
		COMMON_NOT_EVERY_FONT_COLOUR,
		COMMON_BAD_FONT_COLOUR,

		COMMON_NO_DRAW_DEPTH,
		COMMON_BAD_DRAW_DEPTH,

		COMMON_NO_ACTIVE_STATE,
		COMMON_BAD_ACTIVE_STATE,

		COMMON_NO_FUNCTION_INDEX,
		COMMON_BAD_FUNCTION_INDEX,

		COMMON_NO_ENABLED,
		COMMON_BAD_ENABLED,

		//SLIDER
		SLIDER_NO_BOUNDS,
		SLIDER_BAD_BOUNDS,

		SLIDER_NO_STEPS,
		SLIDER_BAD_STEPS,

		SLIDER_NO_DECIMALS,
		SLIDER_BAD_DECIMALS,

		SLIDER_NO_DEFAULT,
		SLIDER_BAD_DEFAULT,

		//CLOSED LIST
		CLOSED_LIST_NO_LIST,
		CLOSED_LIST_BAD_MANAGED_LIST,
		CLOSED_LIST_NO_CHOSEN,
		CLOSED_LIST_BAD_CHOSEN
	};

	template<class Ctrl> struct MenuEntrySeed {
		MenuEntrySeed(const Ctrl& ctrL, float activeStatE, const std::string& namE, bool enableD, int onPressIndeX, int onHoverIndeX, int onReleaseIndeX) :
			ctrl(ctrL),
			activeState(activeStatE),
			enabled(enableD),
			name(namE),
			onPressIndex(onPressIndeX),
			onHoverIndex(onHoverIndeX),
			onReleaseIndex(onReleaseIndeX) {

		}

		Ctrl ctrl;
		float activeState;
		bool enabled;
		std::string name;
		int onPressIndex;//Function index
		int onHoverIndex;//Function index
		int onReleaseIndex;//Function index
	};

	static const unsigned char UNDEF = 0u;
	static const unsigned char BY_DEF = 1u;
	static const unsigned char BY_USER = 2u;

	template<typename T>class ParameterInfo {
		friend struct ControllerInfo;
		friend class MenuParser;
	public:

		using uchar = unsigned char;
		void fillIn(const ParameterInfo<T>& other) {
			if (other.definition >= definition) {
				*this = other;
			}
		};

		void set(const T& cont) {
			container = cont;
			definition = BY_USER;
		};

		T& get() {
			definition = BY_USER;
			return container;
		}

		const T& operator()() const {
			return container;
		}

		uchar getDef() const {
			return definition;
		}

		bool operator!() const {
			return !definition;
		}

	private:
		ParameterInfo(const std::pair<T, uchar>& pair) : container(pair.first), definition(pair.second) {

		};

		T container;
		uchar definition;
	};

	struct ControllerInfo {

		void fillIn(const ControllerInfo& other) {
			//COMMON
			type.fillIn(other.type);
			pos.fillIn(other.pos);
			dim.fillIn(other.dim);
			text.fillIn(other.text);
			textureName.fillIn(other.textureName);
			fontName.fillIn(other.fontName);
			fontSize.fillIn(other.fontSize);
			fontBounds.fillIn(other.fontBounds);
			fontColourNormal.fillIn(other.fontColourNormal);
			fontColourHighlighted.fillIn(other.fontColourHighlighted);
			fontColourUsed.fillIn(other.fontColourUsed);
			fontColourDisabled.fillIn(other.fontColourDisabled);
			drawDepth.fillIn(other.drawDepth);
			activeState.fillIn(other.activeState);
			onPress.fillIn(other.onPress);
			onHover.fillIn(other.onHover);
			onRelease.fillIn(other.onRelease);
			enabled.fillIn(other.enabled);
			//SLIDER
			slider_bounds.fillIn(other.slider_bounds);
			slider_steps.fillIn(other.slider_steps);
			slider_decimals.fillIn(other.slider_decimals);
			slider_default_portion.fillIn(other.slider_default_portion);
			//CLOSED LIST
			closedList_list.fillIn(other.closedList_list);
			closedList_chosen.fillIn(other.closedList_chosen);
		};

		//COMMON
		using uchar = unsigned char;
		ParameterInfo<ControllerType> type = std::make_pair(ControllerType::TEMPLATE, BY_DEF);
		ParameterInfo<glm::vec2> pos = std::make_pair(glm::vec2(0.0f, 0.0f), UNDEF);
		ParameterInfo<glm::vec2> dim = std::make_pair(glm::vec2(0.0f, 0.0f), UNDEF);
		ParameterInfo<std::string> text = std::make_pair(std::string{""}, UNDEF);
		ParameterInfo<std::string> textureName = std::make_pair(std::string{""}, UNDEF);
		ParameterInfo<std::string> fontName = std::make_pair(std::string{""}, UNDEF);
		ParameterInfo<int> fontSize = std::make_pair(-1, UNDEF);
		ParameterInfo<glm::uvec2> fontBounds = std::make_pair(glm::uvec2(FIRST_PRINTABLE_CHAR, LAST_PRINTABLE_CHAR), BY_DEF);
		ParameterInfo<RE::Colour> fontColourNormal = std::make_pair(RE::Colour{}, UNDEF);
		ParameterInfo<RE::Colour> fontColourHighlighted = std::make_pair(RE::Colour{}, UNDEF);
		ParameterInfo<RE::Colour> fontColourUsed = std::make_pair(RE::Colour{}, UNDEF);
		ParameterInfo<RE::Colour> fontColourDisabled = std::make_pair(RE::Colour{}, UNDEF);
		ParameterInfo<int> drawDepth = std::make_pair(0, BY_DEF);
		ParameterInfo<float> activeState = std::make_pair(0.0f, BY_DEF);
		ParameterInfo<int> onPress = std::make_pair(-1, BY_DEF);
		ParameterInfo<int> onHover = std::make_pair(-1, BY_DEF);
		ParameterInfo<int> onRelease = std::make_pair(-1, BY_DEF);
		ParameterInfo<unsigned char> enabled = std::make_pair(uchar{1}, BY_DEF);
		//SLIDER
		ParameterInfo<glm::vec2> slider_bounds = std::make_pair(glm::vec2{}, UNDEF);
		ParameterInfo<float> slider_steps = std::make_pair(101.0f, BY_DEF);
		ParameterInfo<unsigned char> slider_decimals = std::make_pair(uchar{2}, BY_DEF);
		ParameterInfo<float> slider_default_portion = std::make_pair(0.5f, BY_DEF);
		//CLOSED LIST
		ParameterInfo<std::vector<std::string>> closedList_list = std::make_pair(std::vector<std::string>{}, UNDEF);
		ParameterInfo<size_t> closedList_chosen = std::make_pair(size_t{0}, BY_DEF);
	};

	class MenuParser {
	public:
		using str = std::string;
		using uchar = unsigned char;
		using uint = unsigned int;
		using ll = long long;
		using ull = unsigned long long;

		static MenuParserError parseMenu(
			const std::string& file,
			const glm::vec2& windowSize,
			RE::SpriteBatch* spriteBatch,
			RE::TypingHandle typingHandle,
			std::unordered_map<std::string, ControllerInfo>& infoMap,
			std::vector<MenuEntrySeed<Button>>& buttons,
			std::vector<MenuEntrySeed<TextField>>& textFields,
			std::vector<MenuEntrySeed<Slider>>& sliders,
			std::vector<MenuEntrySeed<PlainText>>& plainTexts,
			std::vector<MenuEntrySeed<ClosedList>>& closedLists
			MENU_LM_DEC) {
			std::unordered_set<std::string> fileSet;
			//Recursive file loading
			auto error = parseFile(file, windowSize, infoMap, fileSet MENU_LM_PASS);
			if (error != MenuParserError::OK) {
				return error;
			}

			for (auto& info : infoMap) {
				RE::Colour fontColours[4] = { info.second.fontColourNormal(), info.second.fontColourHighlighted(), info.second.fontColourUsed(), info.second.fontColourDisabled() };
				switch (info.second.type()) {
				case ControllerType::BUTTON: {
					Button ctrl{ info.second.pos(), info.second.dim(), info.second.text(), RE::RM::getTexture(info.second.textureName()), spriteBatch, RE::RM::getFont(RE::FontSeed{ info.second.fontName(), info.second.fontSize(), (Uint16)info.second.fontBounds().x, (Uint16)info.second.fontBounds().y }), fontColours, info.second.drawDepth() };
					if (!info.second.enabled()) { ctrl.disable(); }
					buttons.emplace_back(ctrl, info.second.activeState(), info.first, info.second.enabled(), info.second.onPress(), info.second.onHover(), info.second.onRelease());
					break; }
				case ControllerType::TEXT_FIELD: {
					TextField ctrl{ info.second.pos(), info.second.dim(), info.second.text(), RE::RM::getTexture(info.second.textureName()), spriteBatch, RE::RM::getFont(RE::FontSeed{ info.second.fontName(), info.second.fontSize(), (Uint16)info.second.fontBounds().x, (Uint16)info.second.fontBounds().y }), fontColours, info.second.drawDepth(), typingHandle };
					if (!info.second.enabled()) { ctrl.disable(); }
					textFields.emplace_back(ctrl, info.second.activeState(), info.first, info.second.enabled(), info.second.onPress(), info.second.onHover(), info.second.onRelease());
					break; }
				case ControllerType::SLIDER: {
					Slider ctrl{ info.second.pos(), info.second.dim(), info.second.text(), RE::RM::getTexture(info.second.textureName()), spriteBatch, RE::RM::getFont(RE::FontSeed{ info.second.fontName(), info.second.fontSize(), (Uint16)info.second.fontBounds().x, (Uint16)info.second.fontBounds().y }), fontColours, info.second.drawDepth(), info.second.slider_bounds().x, info.second.slider_bounds().y, info.second.slider_steps(), info.second.slider_decimals() };
					ctrl.setPortion(info.second.slider_default_portion());
					if (!info.second.enabled()) { ctrl.disable(); }
					sliders.emplace_back(ctrl, info.second.activeState(), info.first, info.second.enabled(), info.second.onPress(), info.second.onHover(), info.second.onRelease());
					break; }
				case ControllerType::PLAIN_TEXT: {
					PlainText ctrl{ info.second.pos(), info.second.dim(), info.second.text(), RE::RM::getTexture(info.second.textureName()), spriteBatch, RE::RM::getFont(RE::FontSeed{ info.second.fontName(), info.second.fontSize(), (Uint16)info.second.fontBounds().x, (Uint16)info.second.fontBounds().y }), fontColours, info.second.drawDepth() };
					if (!info.second.enabled()) { ctrl.disable(); }
					plainTexts.emplace_back(ctrl, info.second.activeState(), info.first, info.second.enabled(), info.second.onPress(), info.second.onHover(), info.second.onRelease());
					break; }
				case ControllerType::CLOSED_LIST: {
					ClosedList ctrl{ info.second.pos(), info.second.dim(), RE::RM::getTexture(info.second.textureName()), spriteBatch, RE::RM::getFont(RE::FontSeed{ info.second.fontName(), info.second.fontSize(), (Uint16)info.second.fontBounds().x, (Uint16)info.second.fontBounds().y }), fontColours, info.second.drawDepth(), info.second.closedList_list() };
					ctrl.choose(info.second.closedList_chosen());
					if (!info.second.enabled()) { ctrl.disable(); }
					closedLists.emplace_back(ctrl, info.second.activeState(), info.first, info.second.enabled(), info.second.onPress(), info.second.onHover(), info.second.onRelease());
					break; }
				}
			}
			return MenuParserError::OK;
		}

	private:
		//Recursive file parsing
		//Parses all included files
		static MenuParserError parseFile(const std::string& file,
			const glm::vec2& windowSize,
			std::unordered_map<std::string, ControllerInfo>& infoMap,
			std::unordered_set<std::string>& fileSet
			MENU_LM_DEC) {
			if (fileSet.find(file) != fileSet.end()) {
				//This file is already loaded, skipping it here
				return MenuParserError::OK;
			}
			nlohmann::ordered_json j;
			std::ifstream i(file);

			try {
				i >> j;
			}
			catch (...) {
				return MenuParserError::ERROR_READING_FILE;
			}
			fileSet.insert(file);
			std::string folder = file.substr(0u, file.find_last_of("/") + 1u);
			//Loading header
			if (j.begin().key() == "head") {
				auto sec = *j.begin();
				//Loading included files
				if (sec.find("include") != sec.end()) {
					auto vec = sec.find("include")->get<std::vector<str>>();
					for (auto& name : vec) {
						std::string filepath;
						if ((name.front() == '\'' && name.back() == '\'') || (name.front() == '\"' && name.back() == '\"')) {
							//Exact path
							filepath = name.substr(1u, name.size() - 2u);
						}
						else {
							//Additive path
							filepath = folder + name;
						}
						auto error = parseFile(filepath, windowSize, infoMap, fileSet MENU_LM_PASS);
						if (error != MenuParserError::OK) {
							return error;
						}
					}
				}
			}
			for (auto it = j.begin(); it != j.end(); it++) {
				MenuParserError error = parseController(j, it.key(), windowSize, infoMap MENU_LM_PASS);
				if (error != MenuParserError::OK) {
					return error;
				}
			}

			return MenuParserError::OK;
		};


		static MenuParserError parseController(
			const nlohmann::ordered_json& json,
			const std::string& name,
			const glm::vec2& windowSize,
			std::unordered_map<std::string, ControllerInfo>& infoMap
			MENU_LM_DEC) {
			if (name == "head") {//Skipping header
				return MenuParserError::OK;
			}
			if (json.find(name) == json.end()) {
				return MenuParserError::NONEXISTENT_TEMPLATE;
			}
			auto section = json[name];
			ControllerInfo finalInfo;
			if (section.contains("templates")) {
				auto templates = section["templates"].get<std::vector<str>>();
				//First we insert templates
				for (auto& templ : templates) {
					if (infoMap.count(templ) == 0u) {//This is not loaded yet
						MenuParserError error = parseController(json, templ, windowSize, infoMap MENU_LM_PASS);
						if (error != MenuParserError::OK) {
							return error;
						}
					}
					finalInfo.fillIn(infoMap[templ]);
				}
			}
			MenuParserError error = parseControllerInfo(section, windowSize, finalInfo MENU_LM_PASS);
			if (error != MenuParserError::OK) {
				return error;
			}
			infoMap.insert(std::make_pair(name, finalInfo));
			return MenuParserError::OK;
		}


		//Bool tells if it is optional
		//true = optional
		//false = required
		//Then writes true there if it was present.
		static MenuParserError parseControllerInfo(
			const nlohmann::ordered_json& section,
			const glm::vec2 windowSize,
			ControllerInfo& i
			MENU_LM_DEC) {

			//TYPE
			if (section.contains("type")) {
				std::string typeStr = section["type"].get<std::string>();
				if (typeStr == "template") {
					i.type.set(ControllerType::TEMPLATE);
				}
				else if (typeStr == "button") {
					i.type.set(ControllerType::BUTTON);
				}
				else if (typeStr == "textField" || typeStr == "text_field") {
					i.type.set(ControllerType::TEXT_FIELD);
				}
				else if (typeStr == "slider") {
					i.type.set(ControllerType::SLIDER);
				}
				else if (typeStr == "plainText" || typeStr == "plain_text") {
					i.type.set(ControllerType::PLAIN_TEXT);
				}
				else if (typeStr == "closedList" || typeStr == "closed_list") {
					i.type.set(ControllerType::CLOSED_LIST);
				}
				else {
					return MenuParserError::COMMON_BAD_TYPE;
				}
			}
			else if (!i.type) {
				return MenuParserError::COMMON_NO_TYPE;
			}


			//POSITION
			if (!section.contains("pos") && !section.contains("posRel") && !section.contains("pos_rel") && !i.pos && i.type() != ControllerType::TEMPLATE) {
				return MenuParserError::COMMON_NO_POS;
			}
			try {
				//Abs
				if (section.contains("pos")) {
					auto vec = section["pos"].get<std::vector<float>>();
					i.pos.get() += glm::vec2{ vec[0], vec[1] };
				}
				//Rel
				if (section.contains("posRel")) {
					auto vec = section["posRel"].get<std::vector<float>>();
					i.pos.get() += glm::vec2{ vec[0], vec[1] } *windowSize;
				}
				else if (section.contains("pos_rel")) {
					auto vec = section["pos_rel"].get<std::vector<float>>();
					i.pos.get() += glm::vec2{ vec[0], vec[1] } *windowSize;
				}
			}
			catch (...) {
				return MenuParserError::COMMON_BAD_POS;
			}

			//DIMENSION
			if (!section.contains("dim") && !section.contains("dimRel") && !section.contains("dim_rel") && !i.dim && i.type() != ControllerType::TEMPLATE) {
				return MenuParserError::COMMON_NO_DIM;
			}
			try {
				//Abs
				if (section.contains("dim")) {
					auto vec = section["dim"].get<std::vector<float>>();
					i.dim.get() += glm::vec2{ vec[0], vec[1] };
				}
				//Rel
				if (section.contains("dimRel")) {
					auto vec = section["dimRel"].get<std::vector<float>>();
					i.dim.get() += glm::vec2{ vec[0], vec[1] } *windowSize;
				}
				else if (section.contains("dim_rel")) {
					auto vec = section["dim_rel"].get<std::vector<float>>();
					i.dim.get() += glm::vec2{ vec[0], vec[1] } *windowSize;
				}
			}
			catch (...) {
				return MenuParserError::COMMON_BAD_DIM;
			}

			//TEXT
			if (section.contains("text")) {
				std::string value = section["text"].get<str>();
#ifdef LM_EXPRESSION_LIST
				try {
					size_t pos = value.find("->");
					if ((pos != std::string::npos) && (value[pos - 1] != '\\')) {
						value = LM["e" + value.substr(0, pos) + "_" + value.substr(pos + 2)];
					}
				}
				catch (...) {
					return MenuParserError::COMMON_BAD_MANAGED_TEXT;
				}
#endif // LM_EXPRESSION_LIST
				i.text.set(value);
			}
			else if (!i.text && i.type() != ControllerType::TEMPLATE && i.type() != ControllerType::SLIDER && i.type() != ControllerType::CLOSED_LIST) {
				return MenuParserError::COMMON_NO_TEXT;
			}

			//TEXTURE NAME
			if (section.contains("textureName")) {
				i.textureName.set(section["textureName"].get<str>());
			}
			else if (section.contains("texture_name")) {
				i.textureName.set(section["texture_name"].get<str>());
			}
			else if (!i.textureName && i.type() != ControllerType::TEMPLATE) {
				return MenuParserError::COMMON_NO_TEXTURE_NAME;
			}

			//FONT
			if (section.contains("fontName")) {
				i.fontName.set(section["fontName"].get<str>());
			}
			else if (section.contains("font_name")) {
				i.fontName.set(section["font_name"].get<str>());
			}
			else if (!i.fontName && i.type() != ControllerType::TEMPLATE) {
				return MenuParserError::COMMON_NO_FONT_NAME;
			}

			try {
				if (section.contains("fontSize")) {
					i.fontSize.set(section["fontSize"].get<int>());
				}
				else if (section.contains("font_size")) {
					i.fontSize.set(section["font_size"].get<int>());
				}
				else if (!i.fontSize && i.type() != ControllerType::TEMPLATE) {
					return MenuParserError::COMMON_NO_FONT_SIZE;
				}
			}
			catch (...) {
				return MenuParserError::COMMON_BAD_FONT_SIZE;
			}

			try {
				if (section.contains("fontBounds")) {
					auto vec = section["fontBounds"].get<std::vector<uint>>();
					i.fontBounds.set(glm::uvec2{ (float)vec[0], (float)vec[1] });
				}
				else if (section.contains("font_bounds")) {
					auto vec = section["font_bounds"].get<std::vector<uint>>();
					i.fontBounds.set(glm::uvec2{ (float)vec[0], (float)vec[1] });
				}
				else if (!i.fontBounds && i.type() != ControllerType::TEMPLATE) {
					return MenuParserError::COMMON_NO_FONT_BOUNDS;
				}
			}
			catch (...) {
				return MenuParserError::COMMON_BAD_FONT_BOUNDS;
			}
			bool hadDefault = false;
			if (section.contains("fontColour")) {
				auto vec = section["fontColour"].get<std::vector<uint>>();
				RE::Colour col{ (uchar)vec[0], (uchar)vec[1], (uchar)vec[2], (uchar)vec[3] };
				i.fontColourNormal.set(col);
				i.fontColourHighlighted.set(col);
				i.fontColourUsed.set(col);
				i.fontColourDisabled.set(col);
				hadDefault = true;
			}
			else if (section.contains("font_colour")) {
				auto vec = section["font_colour"].get<std::vector<uint>>();
				RE::Colour col{ (uchar)vec[0], (uchar)vec[1], (uchar)vec[2], (uchar)vec[3] };
				i.fontColourNormal.set(col);
				i.fontColourHighlighted.set(col);
				i.fontColourUsed.set(col);
				i.fontColourDisabled.set(col);
				hadDefault = true;
			}
			if (!hadDefault && i.type() != ControllerType::TEMPLATE
				&& ((!section.contains("fontColourNormal") && !section.contains("font_colour_normal") && !i.fontColourNormal)
					|| (!section.contains("fontColourHighlighted") && !section.contains("font_colour_highlighted") && !i.fontColourHighlighted)
					|| (!section.contains("fontColourUsed") && !section.contains("font_colour_used") && !i.fontColourUsed)
					|| (!section.contains("fontColourDisabled") && !section.contains("font_colour_disabled") && !i.fontColourDisabled))) {
				return MenuParserError::COMMON_NOT_EVERY_FONT_COLOUR;
			}
			try {
				std::vector<uint> vec;
				if (section.contains("fontColourNormal")) {
					vec = section["fontColourNormal"].get<std::vector<uint>>();
					i.fontColourNormal.set(RE::Colour{ (uchar)vec[0], (uchar)vec[1], (uchar)vec[2], (uchar)vec[3] });
				}
				else if (section.contains("font_colour_normal")) {
					vec = section["font_colour_normal"].get<std::vector<uint>>();
					i.fontColourNormal.set(RE::Colour{ (uchar)vec[0], (uchar)vec[1], (uchar)vec[2], (uchar)vec[3] });
				}

				if (section.contains("fontColourHighlighted")) {
					vec = section["fontColourHighlighted"].get<std::vector<uint>>();
					i.fontColourHighlighted.set(RE::Colour{ (uchar)vec[0], (uchar)vec[1], (uchar)vec[2], (uchar)vec[3] });
				}
				else if (section.contains("font_colour_highlighted")) {
					vec = section["font_colour_highlighted"].get<std::vector<uint>>();
					i.fontColourHighlighted.set(RE::Colour{ (uchar)vec[0], (uchar)vec[1], (uchar)vec[2], (uchar)vec[3] });
				}

				if (section.contains("fontColourUsed")) {
					vec = section["fontColourUsed"].get<std::vector<uint>>();
					i.fontColourUsed.set(RE::Colour{ (uchar)vec[0], (uchar)vec[1], (uchar)vec[2], (uchar)vec[3] });
				}
				else if (section.contains("font_colour_used")) {
					vec = section["font_colour_used"].get<std::vector<uint>>();
					i.fontColourUsed.set(RE::Colour{ (uchar)vec[0], (uchar)vec[1], (uchar)vec[2], (uchar)vec[3] });
				}

				if (section.contains("fontColourDisabled")) {
					vec = section["fontColourDisabled"].get<std::vector<uint>>();
					i.fontColourDisabled.set(RE::Colour{ (uchar)vec[0], (uchar)vec[1], (uchar)vec[2], (uchar)vec[3] });
				}
				else if (section.contains("font_colour_disabled")) {
					vec = section["font_colour_disabled"].get<std::vector<uint>>();
					i.fontColourDisabled.set(RE::Colour{ (uchar)vec[0], (uchar)vec[1], (uchar)vec[2], (uchar)vec[3] });
				}
			}
			catch (...) {
				return MenuParserError::COMMON_BAD_FONT_COLOUR;
			}

			//DEPTH
			try {
				if (section.contains("drawDepth")) {
					i.drawDepth.set((int)section["drawDepth"].get<int>());
				}
				else if (section.contains("draw_depth")) {
					i.drawDepth.set((int)section["draw_depth"].get<int>());
				}
				else if (!i.drawDepth && i.type() != ControllerType::TEMPLATE) {
					return MenuParserError::COMMON_NO_DRAW_DEPTH;
				}
			}
			catch (...) {
				return MenuParserError::COMMON_BAD_DRAW_DEPTH;
			}

			//ACTIVE STATE
			try {
				if (section.contains("activeState")) {
					i.activeState.set((float)section["activeState"].get<int>());
				}
				else if (section.contains("active_state")) {
					i.activeState.set((float)section["active_state"].get<int>());
				}
				else if (!i.activeState && i.type() != ControllerType::TEMPLATE) {
					return MenuParserError::COMMON_NO_ACTIVE_STATE;
				}
			}
			catch (...) {
				return MenuParserError::COMMON_BAD_ACTIVE_STATE;
			}

			//FUNCTIONS
			try {
				if (section.contains("onPress")) {
					i.onPress.set((int)section["onPress"].get<int>());
				}
				else if (section.contains("on_press")) {
					i.onPress.set((int)section["on_press"].get<int>());
				}
				else if (!i.onPress && i.type() != ControllerType::TEMPLATE) {
					return MenuParserError::COMMON_NO_FUNCTION_INDEX;
				}

				if (section.contains("onHover")) {
					i.onHover.set((int)section["onHover"].get<int>());
				}
				else if (section.contains("on_hover")) {
					i.onHover.set((int)section["on_hover"].get<int>());
				}
				else if (!i.onPress && i.type() != ControllerType::TEMPLATE) {
					return MenuParserError::COMMON_NO_FUNCTION_INDEX;
				}

				if (section.contains("onRelease")) {
					i.onRelease.set((int)section["onRelease"].get<int>());
				}
				else if (section.contains("on_release")) {
					i.onRelease.set((int)section["on_release"].get<int>());
				}
				else if (!i.onPress && i.type() != ControllerType::TEMPLATE) {
					return MenuParserError::COMMON_NO_FUNCTION_INDEX;
				}
			}
			catch (...) {
				return MenuParserError::COMMON_BAD_FUNCTION_INDEX;
			}

			try {
				if (section.contains("enabled")) {
					i.enabled.set(section["enabled"].get<bool>());
				}
				else if (!i.enabled && i.type() != ControllerType::TEMPLATE) {
					return MenuParserError::COMMON_NO_ENABLED;
				}
			}
			catch (...) {
				return MenuParserError::COMMON_BAD_ENABLED;
			}


			//SLIDER SPECIFIC \|/ (or as template)

			//BOUNDS
			try {
				if (section.contains("slider_bounds")) {
					auto vec = section["slider_bounds"].get<std::vector<float>>();
					i.slider_bounds.set(glm::vec2((float)vec[0], (float)vec[1]));
				}
				else if (!i.slider_bounds && i.type() == ControllerType::SLIDER) {
					return MenuParserError::SLIDER_NO_BOUNDS;
				}
			}
			catch (...) {
			}
			//STEPS
			try {
				if (section.contains("slider_steps")) {
					i.slider_steps.set((float)section["slider_steps"].get<float>());
				}
				else if (!i.slider_steps && i.type() == ControllerType::SLIDER) {
					return MenuParserError::SLIDER_NO_STEPS;
				}
			}
			catch (...) {
				return MenuParserError::SLIDER_BAD_STEPS;
			}
			//DECIMALS
			try {
				if (section.contains("slider_decimals")) {
					i.slider_decimals.set((uchar)section["slider_decimals"].get<uint>());
				}
				else if (!i.slider_decimals && i.type() == ControllerType::SLIDER) {
					return MenuParserError::SLIDER_NO_DECIMALS;
				}
			}
			catch (...) {
				return MenuParserError::SLIDER_BAD_DECIMALS;
			}
			//DEFAULT
			try {
				if (section.contains("slider_defaultPortion")) {
					i.slider_default_portion.set((float)section["slider_defaultPortion"].get<float>());
				}
				else if (section.contains("slider_default_portion")) {
					i.slider_default_portion.set((float)section["slider_default_portion"].get<float>());
				}
				else if (!i.slider_bounds && i.type() == ControllerType::SLIDER) {
					return MenuParserError::SLIDER_NO_DEFAULT;
				}
			}
			catch (...) {
				return MenuParserError::SLIDER_BAD_DEFAULT;
			}

			//CLOSED LIST SPECIFIC \|/ (or as template)

			//LIST
			if (section.contains("closedList_list")) {
				auto vec = section["closedList_list"].get<std::vector<str>>();
				std::vector<std::string> list;
				for (auto& value : vec) {
					std::string s = value;
#ifdef LM_EXPRESSION_LIST
					try {
						size_t pos = s.find("->");
						if ((pos != std::string::npos) && (s[pos - 1] != '\\')) {
							s = LM["e" + s.substr(0, pos) + "_" + s.substr(pos + 2)];
						}
					}
					catch (...) {
						return MenuParserError::CLOSED_LIST_BAD_MANAGED_LIST;
					}
#endif // LM_EXPRESSION_LIST
					list.push_back(s);
				}
				i.closedList_list.set(list);
			}
			else if (section.contains("closed_list_list")) {
				auto vec = section["closed_list_list"].get<std::vector<str>>();
				std::vector<std::string> list;
				for (auto& value : vec) {
					std::string s = value;
#ifdef LM_EXPRESSION_LIST
					try {
						size_t pos = s.find("->");
						if ((pos != std::string::npos) && (s[pos - 1] != '\\')) {
							s = LM["e" + s.substr(0, pos) + "_" + s.substr(pos + 2)];
						}
					}
					catch (...) {
						return MenuParserError::CLOSED_LIST_BAD_MANAGED_LIST;
					}
#endif // LM_EXPRESSION_LIST
					list.push_back(s);
				}
				i.closedList_list.set(list);
			}
			else if (!i.closedList_list && i.type() == ControllerType::CLOSED_LIST) {
				return MenuParserError::CLOSED_LIST_NO_LIST;
			}

			//CHOSEN
			try {
				if (section.contains("closedList_chosen")) {
					i.closedList_chosen.set((size_t)section["closedList_chosen"].get<uint>());
					if (i.closedList_chosen() >= i.closedList_list().size()) { throw 1; }
				}
				else if (section.contains("closed_list_chosen")) {
					i.closedList_chosen.set((size_t)section["closed_list_chosen"].get<uint>());
					if (i.closedList_chosen() >= i.closedList_list().size()) { throw 1; }
				}
				else if (!i.closedList_chosen && i.type() == ControllerType::CLOSED_LIST) {
					return MenuParserError::CLOSED_LIST_NO_CHOSEN;
				}
			}
			catch (...) {
				return MenuParserError::CLOSED_LIST_BAD_CHOSEN;
			}

			return MenuParserError::OK;
		}
	};

}