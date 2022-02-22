#pragma once
#include <vector>
#include <unordered_map>
#include <limits>
#include <iostream>

#include <nlohmann/json.hpp>
#include <SDL2/SDL_timer.h>

#include <RealEngine/utility/tupleUp.hpp>
#include <RealEngine/graphics/CursorDrawer.hpp>
#include <RealEngine/user_input/TypingHandle.hpp>
#include <RealEngine/main/MainProgram.hpp>


#include <RealEngine/GUI/MenuParser.hpp>
#include <RealEngine/GUI/Button.hpp>
#include <RealEngine/GUI/TextField.hpp>
#include <RealEngine/GUI/Slider.hpp>
#include <RealEngine/GUI/PlainText.hpp>
#include <RealEngine/GUI/ClosedList.hpp>

#ifdef LM_EXPRESSION_LIST
#define MENU_LM_DEC , RE::LanguageManager LM
#define MENU_LM_PASS , LM
#include <RealEngine/utility/LanguageManager.hpp>
#else
#define MENU_LM_DEC
#define MENU_LM_PASS
#endif // LM_EXPRESSION_LIST


namespace RGUI {

const float ALWAYS_ACTIVE = 0.0f;

struct MenuIndex {
	MenuIndex() {}
	MenuIndex(ControllerType type, size_t index) : type(type), index(index) {}

	size_t index;
	ControllerType type;
};

template <class Ctrl, class Observer> struct MenuEntry {
	MenuEntry(const Ctrl& c, float activeState, const std::string& name, bool enabled = true,
		void (Observer::* onPress)(const std::string&) = nullptr,
		void (Observer::* onHover)(const std::string&) = nullptr,
		void (Observer::* onRelease)(const std::string&) = nullptr) : c(c), activeState(activeState), name(name), enabled(enabled), onPress(onPress), onHover(onHover), onRelease(onRelease) {}
	/*
	Controller is active when:
	a) activeState == ALWAYS_ACTIVE
	b) activeState > 0.0f && activeState == currentState
	c) activeState < 0.0f && activeState != currentState
	*/
	bool isActive(float currentState) const {
		if (((activeState > ALWAYS_ACTIVE) ? (activeState == currentState) : (activeState != -currentState)) || activeState == ALWAYS_ACTIVE) {
			return true;
		}
		return false;
	}

	Ctrl c;
	std::string name;
	float activeState;//State when this controller is active
	void (Observer::* onPress)(const std::string&);
	void (Observer::* onHover)(const std::string&);
	void (Observer::* onRelease)(const std::string&);
	bool enabled = true;
};

template<class Observer> class Menu {
public:

	Menu() {

	}

	Menu(RE::MainProgram* mainProgram, Observer* observer) :
		m_observer(observer)
	#ifdef RE_MENU_KEEP_INFOS
		,
		m_typingHandle(mainProgram)
	#endif
	{

	}

	void init(Observer* observer) {
		m_observer = observer;
	};

	//Returns nullptr if such controller does not exist
	template<class Ctrl>Ctrl* getController(const std::string& name) {
		auto it = m_names.find(name);
		if (it == m_names.end()) {
			return nullptr;
		}

	#ifdef _DEBUG
		switch (it->second.type) {
		case ControllerType::BUTTON:
			if (!std::is_same<Ctrl, Button>::value) { return nullptr; }
			break;
		case ControllerType::TEXT_FIELD:
			if (!std::is_same<Ctrl, TextField>::value) { return nullptr; }
			break;
		case ControllerType::SLIDER:
			if (!std::is_same<Ctrl, Slider>::value) { return nullptr; }
			break;
		case ControllerType::PLAIN_TEXT:
			if (!std::is_same<Ctrl, PlainText>::value) { return nullptr; }
			break;
		case ControllerType::CLOSED_LIST:
			if (!std::is_same<Ctrl, ClosedList>::value) { return nullptr; }
			break;
		default:
			return nullptr;
		}
	#endif // _DEBUG

		return &std::get<std::vector<MenuEntry<Ctrl, Observer>>>(m_controllers)[it->second.index].c;
	};

	//Returns true if successfully enabled (false = such controller does not exist etc.)
	bool enable(const std::string& name) {
		auto it = m_names.find(name);
		if (it == m_names.end()) {
			return false;
		}
		switch (it->second.type) {
		case ControllerType::BUTTON: {
			auto& entry = std::get<std::vector<MenuEntry<Button, Observer>>>(m_controllers)[it->second.index];
			entry.c.enable();
			entry.enabled = true;
			break; }
		case ControllerType::TEXT_FIELD: {
			auto& entry = std::get<std::vector<MenuEntry<TextField, Observer>>>(m_controllers)[it->second.index];
			entry.c.enable();
			entry.enabled = true;
			break; }
		case ControllerType::SLIDER: {
			auto& entry = std::get<std::vector<MenuEntry<Slider, Observer>>>(m_controllers)[it->second.index];
			entry.c.enable();
			entry.enabled = true;
			break; }
		case ControllerType::PLAIN_TEXT: {
			auto& entry = std::get<std::vector<MenuEntry<PlainText, Observer>>>(m_controllers)[it->second.index];
			entry.c.enable();
			entry.enabled = true;
			break; }
		case ControllerType::CLOSED_LIST: {
			auto& entry = std::get<std::vector<MenuEntry<ClosedList, Observer>>>(m_controllers)[it->second.index];
			entry.c.enable();
			entry.enabled = true;
			break; }
		default:
			return false;
		}
		return true;
	};

	//Returns true if successfully disabled (false = such controller does not exist etc.)
	bool disable(const std::string& name) {
		auto it = m_names.find(name);
		if (it == m_names.end()) {
			return false;
		}
		switch (it->second.type) {
		case ControllerType::BUTTON: {
			auto& entry = std::get<std::vector<MenuEntry<Button, Observer>>>(m_controllers)[it->second.index];
			entry.c.disable();
			entry.enabled = false;
			break; }
		case ControllerType::TEXT_FIELD: {
			auto& entry = std::get<std::vector<MenuEntry<TextField, Observer>>>(m_controllers)[it->second.index];
			entry.c.disable();
			entry.enabled = false;
			break; }
		case ControllerType::SLIDER: {
			auto& entry = std::get<std::vector<MenuEntry<Slider, Observer>>>(m_controllers)[it->second.index];
			entry.c.disable();
			entry.enabled = false;
			break; }
		case ControllerType::PLAIN_TEXT: {
			auto& entry = std::get<std::vector<MenuEntry<PlainText, Observer>>>(m_controllers)[it->second.index];
			entry.c.disable();
			entry.enabled = false;
			break; }
		case ControllerType::CLOSED_LIST: {
			auto& entry = std::get<std::vector<MenuEntry<ClosedList, Observer>>>(m_controllers)[it->second.index];
			entry.c.disable();
			entry.enabled = false;
			break; }
		default:
			return false;
		}
		return true;
	};

	//Returns true if successfully removed the controller (false = such controller does not exist etc.)
	bool remove(const std::string& name) {
		auto it = m_names.find(name);
		if (it == m_names.end()) {
			return false;
		}
		switch (it->second.type) {
		case ControllerType::BUTTON: {
			auto& vec = std::get<std::vector<MenuEntry<Button, Observer>>>(m_controllers);
			m_names.find(vec.back().name)->second.index = it->second.index;
			vec[it->second.index] = vec.back();
			vec.pop_back();
			break; }
		case ControllerType::TEXT_FIELD: {
			auto& vec = std::get<std::vector<MenuEntry<TextField, Observer>>>(m_controllers);
			m_names.find(vec.back().name)->second.index = it->second.index;
			vec[it->second.index] = vec.back();
			vec.pop_back();
			break; }
		case ControllerType::SLIDER: {
			auto& vec = std::get<std::vector<MenuEntry<Slider, Observer>>>(m_controllers);
			m_names.find(vec.back().name)->second.index = it->second.index;
			vec[it->second.index] = vec.back();
			vec.pop_back();
			break; }
		case ControllerType::PLAIN_TEXT: {
			auto& vec = std::get<std::vector<MenuEntry<PlainText, Observer>>>(m_controllers);
			m_names.find(vec.back().name)->second.index = it->second.index;
			vec[it->second.index] = vec.back();
			vec.pop_back();
			break; }
		case ControllerType::CLOSED_LIST: {
			auto& vec = std::get<std::vector<MenuEntry<ClosedList, Observer>>>(m_controllers);
			m_names.find(vec.back().name)->second.index = it->second.index;
			vec[it->second.index] = vec.back();
			vec.pop_back();
			break; }
		default:
			return false;
		}
		m_names.erase(it);
		return true;
	};

	//Returns true if successfully added the controller (false = a controller with such name already exists etc.)
	template<class Ctrl>bool addController(const Ctrl& c, float activeState,
		const std::string& name,
		void (Observer::* onPress)(const std::string&) = nullptr,
		void (Observer::* onHover)(const std::string&) = nullptr,
		void (Observer::* onRelease)(const std::string&) = nullptr) {
		auto it = m_names.find(name);
		if (it != m_names.end()) {
			//Controller with such name already exists
			return false;
		}
		auto& vec = std::get<std::vector<MenuEntry<Ctrl, Observer>>>(m_controllers);
		size_t index = vec.size();
		vec.emplace_back(c, activeState, name, true, onPress, onHover, onRelease);
		ControllerType type;

		if (std::is_same<Ctrl, Button>::value)  type = ControllerType::BUTTON;
		else if (std::is_same<Ctrl, TextField>::value)  type = ControllerType::TEXT_FIELD;
		else if (std::is_same<Ctrl, Slider>::value)  type = ControllerType::SLIDER;
		else if (std::is_same<Ctrl, PlainText>::value)  type = ControllerType::PLAIN_TEXT;
		else if (std::is_same<Ctrl, ClosedList>::value)  type = ControllerType::CLOSED_LIST;
		else static_assert(true, "Controllers in menu can only have type of: RGUI::Button, RGUI::TextField, RGUI::Slider, RGUI::PlainText, RGUI::ClosedList");

		m_names.insert(std::make_pair(name, MenuIndex{type, index}));
		return true;
	};

	//Returns true if successfully added the controller (false = a controller with such name already exists etc.)
	//No function index validation!
#ifdef RE_MENU_KEEP_INFOS
	bool addController(const std::string& name, const ControllerInfo& info) {
		auto it = m_names.find(name);
		if (it != m_names.end()) {
			//Controller with such name already exists
			return false;
		}

		func onPress = getFunctionFromIndex(m_functions, info.onPress());
		func onHover = getFunctionFromIndex(m_functions, info.onHover());
		func onRelease = getFunctionFromIndex(m_functions, info.onRelease());

		RE::Colour fontColours[4] = {info.fontColourNormal(), info.fontColourHighlighted(), info.fontColourUsed(), info.fontColourDisabled()};
		size_t index;
		switch (info.type()) {
		case ControllerType::BUTTON: {
			Button ctrl{info.pos(), info.dim(), info.text(), RE::RM::getTexture(info.textureName()), m_spriteBatch, RE::RM::getFont(RE::FontSeed{ info.fontName(), info.fontSize(), info.fontBounds().x, info.fontBounds().y }), fontColours, info.drawDepth()};
			if (!info.enabled()) { ctrl.disable(); }
			auto& vec = std::get<std::vector<MenuEntry<decltype(ctrl), Observer>>>(m_controllers);
			index = vec.size();
			vec.emplace_back(ctrl, info.activeState(), name, info.enabled(), onPress, onHover, onRelease);
			break; }
		case ControllerType::TEXT_FIELD: {
			TextField ctrl{info.pos(), info.dim(), info.text(), RE::RM::getTexture(info.textureName()), m_spriteBatch, RE::RM::getFont(RE::FontSeed{ info.fontName(), info.fontSize(), info.fontBounds().x, info.fontBounds().y }), fontColours, info.drawDepth(), m_typingHandle};
			if (!info.enabled()) { ctrl.disable(); }
			auto& vec = std::get<std::vector<MenuEntry<decltype(ctrl), Observer>>>(m_controllers);
			index = vec.size();
			vec.emplace_back(ctrl, info.activeState(), name, info.enabled(), onPress, onHover, onRelease);
			break; }
		case ControllerType::SLIDER: {
			Slider ctrl{info.pos(), info.dim(), info.text(), RE::RM::getTexture(info.textureName()), m_spriteBatch, RE::RM::getFont(RE::FontSeed{ info.fontName(), info.fontSize(), info.fontBounds().x, info.fontBounds().y }), fontColours, info.drawDepth(), info.slider_bounds().x, info.slider_bounds().y, info.slider_steps(), info.slider_decimals()};
			ctrl.setPortion(info.slider_default_portion());
			if (!info.enabled()) { ctrl.disable(); }
			auto& vec = std::get<std::vector<MenuEntry<decltype(ctrl), Observer>>>(m_controllers);
			index = vec.size();
			vec.emplace_back(ctrl, info.activeState(), name, info.enabled(), onPress, onHover, onRelease);
			break; }
		case ControllerType::PLAIN_TEXT: {
			PlainText ctrl{info.pos(), info.dim(), info.text(), RE::RM::getTexture(info.textureName()), m_spriteBatch, RE::RM::getFont(RE::FontSeed{ info.fontName(), info.fontSize(), info.fontBounds().x, info.fontBounds().y }), fontColours, info.drawDepth()};
			if (!info.enabled()) { ctrl.disable(); }
			auto& vec = std::get<std::vector<MenuEntry<decltype(ctrl), Observer>>>(m_controllers);
			index = vec.size();
			vec.emplace_back(ctrl, info.activeState(), name, info.enabled(), onPress, onHover, onRelease);
			break; }
		case ControllerType::CLOSED_LIST: {
			ClosedList ctrl{info.pos(), info.dim(), RE::RM::getTexture(info.textureName()), m_spriteBatch, RE::RM::getFont(RE::FontSeed{ info.fontName(), info.fontSize(), info.fontBounds().x, info.fontBounds().y }), fontColours, info.drawDepth(), info.closedList_list()};
			ctrl.choose(info.closedList_chosen());
			if (!info.enabled()) { ctrl.disable(); }
			auto& vec = std::get<std::vector<MenuEntry<decltype(ctrl), Observer>>>(m_controllers);
			index = vec.size();
			vec.emplace_back(ctrl, info.activeState(), name, info.enabled(), onPress, onHover, onRelease);
			break; }
		default:
			//Unknown type
			return false;
		}
		m_names.insert(std::make_pair(name, MenuIndex(info.type(), index)));
		return true;
	};
#endif

#ifdef RE_MENU_KEEP_INFOS
	//Throws if such info does not exist
	const ControllerInfo& getInfo(const std::string& name) {
		auto it = m_infoMap.find(name);
		if (it == m_infoMap.end()) {
			//Info with such name does not exists
			throw;
		}
		return it->second;
	};
#endif

	void clear() {
		std::get<std::vector<MenuEntry<Button, Observer>>>(m_controllers).clear();
		std::get<std::vector<MenuEntry<TextField, Observer>>>(m_controllers).clear();
		std::get<std::vector<MenuEntry<Slider, Observer>>>(m_controllers).clear();
		std::get<std::vector<MenuEntry<PlainText, Observer>>>(m_controllers).clear();
		std::get<std::vector<MenuEntry<ClosedList, Observer>>>(m_controllers).clear();
		m_names.clear();
	};

	//windowSize -> used for window-relative coords and dims
	MenuParserError loadMenuFromFile(const std::string& filePath,
		const glm::vec2& windowSize, RE::SpriteBatch* spriteBatch,
		std::vector<void (Observer::*)(const std::string&)>& functions
		MENU_LM_DEC) {
		clear();
		Uint32 start = SDL_GetTicks();
		std::vector<MenuEntrySeed<Button>> buttons;
		std::vector<MenuEntrySeed<TextField>> textFields;
		std::vector<MenuEntrySeed<Slider>> sliders;
		std::vector<MenuEntrySeed<PlainText>> plainTexts;
		std::vector<MenuEntrySeed<ClosedList>> closedLists;
		MenuParserError error;
		std::unordered_map<std::string, ControllerInfo> infoMap;
		error = MenuParser::parseMenu(filePath, windowSize, spriteBatch, m_typingHandle, infoMap, buttons, textFields, sliders, plainTexts, closedLists MENU_LM_PASS);
		if (error != MenuParserError::OK) {
			//Parsing failed - throwing error
			return error;
		}
	#ifdef RE_MENU_KEEP_INFOS
		//Saving objects
		m_infoMap = infoMap;
		m_functions = functions;
		m_spriteBatch = spriteBatch;
	#endif
		//BUTTONS
		for (auto& seed : buttons) {
			void (Observer:: * onPressF)(const std::string&);
			void (Observer:: * onHoverF)(const std::string&);
			void (Observer:: * onReleaseF)(const std::string&);
			try {
				onPressF = getFunctionFromIndex(functions, seed.onPressIndex);
				onHoverF = getFunctionFromIndex(functions, seed.onHoverIndex);
				onReleaseF = getFunctionFromIndex(functions, seed.onReleaseIndex);
			}
			catch (...) {
				return MenuParserError::COMMON_BAD_FUNCTION_INDEX;
			}
			size_t index = std::get<std::vector<MenuEntry<Button, Observer>>>(m_controllers).size();
			std::get<std::vector<MenuEntry<Button, Observer>>>(m_controllers).emplace_back(seed.ctrl, seed.activeState, seed.name, seed.enabled, onPressF, onHoverF, onReleaseF);
			m_names.emplace(seed.name, MenuIndex{ControllerType::BUTTON, index});
		}
		//TEXT FIELDS
		for (auto& seed : textFields) {
			void (Observer:: * onPressF)(const std::string&);
			void (Observer:: * onHoverF)(const std::string&);
			void (Observer:: * onReleaseF)(const std::string&);
			try {
				onPressF = getFunctionFromIndex(functions, seed.onPressIndex);
				onHoverF = getFunctionFromIndex(functions, seed.onHoverIndex);
				onReleaseF = getFunctionFromIndex(functions, seed.onReleaseIndex);
			}
			catch (...) {
				return MenuParserError::COMMON_BAD_FUNCTION_INDEX;
			}
			size_t index = std::get<std::vector<MenuEntry<TextField, Observer>>>(m_controllers).size();
			std::get<std::vector<MenuEntry<TextField, Observer>>>(m_controllers).emplace_back(seed.ctrl, seed.activeState, seed.name, seed.enabled, onPressF, onHoverF, onReleaseF);
			m_names.emplace(seed.name, MenuIndex{ControllerType::TEXT_FIELD, index});
		}
		//SLIDERS
		for (auto& seed : sliders) {
			void (Observer:: * onPressF)(const std::string&);
			void (Observer:: * onHoverF)(const std::string&);
			void (Observer:: * onReleaseF)(const std::string&);
			try {
				onPressF = getFunctionFromIndex(functions, seed.onPressIndex);
				onHoverF = getFunctionFromIndex(functions, seed.onHoverIndex);
				onReleaseF = getFunctionFromIndex(functions, seed.onReleaseIndex);
			}
			catch (...) {
				return MenuParserError::COMMON_BAD_FUNCTION_INDEX;
			}
			size_t index = std::get<std::vector<MenuEntry<Slider, Observer>>>(m_controllers).size();
			std::get<std::vector<MenuEntry<Slider, Observer>>>(m_controllers).emplace_back(seed.ctrl, seed.activeState, seed.name, seed.enabled, onPressF, onHoverF, onReleaseF);
			m_names.emplace(seed.name, MenuIndex{ControllerType::SLIDER, index});
		}
		//PLAIN TEXTS
		for (auto& seed : plainTexts) {
			void (Observer:: * onPressF)(const std::string&);
			void (Observer:: * onHoverF)(const std::string&);
			void (Observer:: * onReleaseF)(const std::string&);
			try {
				onPressF = getFunctionFromIndex(functions, seed.onPressIndex);
				onHoverF = getFunctionFromIndex(functions, seed.onHoverIndex);
				onReleaseF = getFunctionFromIndex(functions, seed.onReleaseIndex);
			}
			catch (...) {
				return MenuParserError::COMMON_BAD_FUNCTION_INDEX;
			}
			size_t index = std::get<std::vector<MenuEntry<PlainText, Observer>>>(m_controllers).size();
			std::get<std::vector<MenuEntry<PlainText, Observer>>>(m_controllers).emplace_back(seed.ctrl, seed.activeState, seed.name, seed.enabled, onPressF, onHoverF, onReleaseF);
			m_names.emplace(seed.name, MenuIndex{ControllerType::PLAIN_TEXT, index});
		}
		//CLOSED LISTS
		for (auto& seed : closedLists) {
			void (Observer:: * onPressF)(const std::string&);
			void (Observer:: * onHoverF)(const std::string&);
			void (Observer:: * onReleaseF)(const std::string&);
			try {
				onPressF = getFunctionFromIndex(functions, seed.onPressIndex);
				onHoverF = getFunctionFromIndex(functions, seed.onHoverIndex);
				onReleaseF = getFunctionFromIndex(functions, seed.onReleaseIndex);
			}
			catch (...) {
				return MenuParserError::COMMON_BAD_FUNCTION_INDEX;
			}
			size_t index = std::get<std::vector<MenuEntry<ClosedList, Observer>>>(m_controllers).size();
			std::get<std::vector<MenuEntry<ClosedList, Observer>>>(m_controllers).emplace_back(seed.ctrl, seed.activeState, seed.name, seed.enabled, onPressF, onHoverF, onReleaseF);
			m_names.emplace(seed.name, MenuIndex{ControllerType::CLOSED_LIST, index});
		}

		std::cout << "Loaded menu '" + filePath + "' in " + std::to_string(SDL_GetTicks() - start) + " ms.\n";
		return MenuParserError::OK;
	}

	float getState() const {
		return m_currentState;
	}

	void setState(float newState) {
		m_currentState = newState;
		RE::CursorDrawer::setCursor(RE::CURSOR_STATE::ARROW);
	}

	//Returns true if clicked on any controller
	bool onPress(const glm::vec2& cursorPos) {
		//ClosedList should go first
		if (forActiveOverlappedController<ClosedList>(cursorPos, &ClosedList::press, &ClosedList::onGlobalPress, &MenuEntry<ClosedList, Observer>::onPress)) { return true; }
		//Then slider
		if (forActiveOverlappedController<Slider>(cursorPos, &Slider::press, &Slider::onGlobalPress, &MenuEntry<Slider, Observer>::onPress)) { return true; }
		if (forActiveOverlappedController<Button>(cursorPos, &Button::press, &Button::onGlobalPress, &MenuEntry<Button, Observer>::onPress)) { return true; }
		if (forActiveOverlappedController<TextField>(cursorPos, &TextField::press, &TextField::onGlobalPress, &MenuEntry<TextField, Observer>::onPress)) { return true; }
		return false;
	}
	//Returns true if hovering on any controller
	bool step(const glm::vec2& cursorPos) {
		//ClosedList should go first
		if (forActiveOverlappedController<ClosedList>(cursorPos, &ClosedList::hover, &ClosedList::step, &MenuEntry<ClosedList, Observer>::onHover)) { return true; }
		//Then slider
		if (forActiveOverlappedController<Slider>(cursorPos, &Slider::hover, &Slider::step, &MenuEntry<Slider, Observer>::onHover)) { return true; }
		if (forActiveOverlappedController<Button>(cursorPos, &Button::hover, &Button::step, &MenuEntry<Button, Observer>::onHover)) { return true; }
		if (forActiveOverlappedController<TextField>(cursorPos, &TextField::hover, &TextField::step, &MenuEntry<TextField, Observer>::onHover)) { return true; }
		return false;
	}
	//Returns true if clicked on any controller
	bool onRelease(const glm::vec2& cursorPos) {
		//ClosedList should go first
		if (forActiveOverlappedController<ClosedList>(cursorPos, &ClosedList::release, &ClosedList::onGlobalRelease, &MenuEntry<ClosedList, Observer>::onRelease)) { return true; }
		//Then slider
		if (forActiveOverlappedController<Slider>(cursorPos, &Slider::release, &Slider::onGlobalRelease, &MenuEntry<Slider, Observer>::onRelease)) { return true; }
		if (forActiveOverlappedController<Button>(cursorPos, &Button::release, &Button::onGlobalRelease, &MenuEntry<Button, Observer>::onRelease)) { return true; }
		if (forActiveOverlappedController<TextField>(cursorPos, &TextField::release, &TextField::onGlobalRelease, &MenuEntry<TextField, Observer>::onRelease)) { return true; }
		return false;
	}

	void draw() const {
		cforActiveControllerDisabledToo<Button>(&Button::draw);
		cforActiveControllerDisabledToo<TextField>(&TextField::draw);
		cforActiveControllerDisabledToo<Slider>(&Slider::draw);
		cforActiveControllerDisabledToo<PlainText>(&PlainText::draw);
		cforActiveControllerDisabledToo<ClosedList>(&ClosedList::draw);
	}

private:
	using func = void (Observer::*)(const std::string&);

	func getFunctionFromIndex(std::vector<void(Observer::*)(const std::string&)>& functions, int index) {
		if (index == -1) {
			return nullptr;
		} else {
			if (index >= 0 && index < (int)functions.size()) {
				return functions[index];
			} else {
				throw MenuParserError::COMMON_BAD_FUNCTION_INDEX;
			}
		}
	};

	//Non-constant
	//Returns if overlapped at least one controller
	template<class Ctrl> bool forActiveOverlappedController(const glm::vec2& cursorPos, bool (Ctrl::* evntOverlapped)(const glm::vec2&), void (Ctrl::* evntOtherwise)(const glm::vec2&), void (Observer::* MenuEntry<Ctrl, Observer>::* onEvent)(const std::string&)) {
		for (auto& entry : std::get<std::vector<MenuEntry<Ctrl, Observer>>>(m_controllers)) {
			if (entry.isActive(m_currentState) && entry.enabled) {
				if ((entry.c.*evntOverlapped)(cursorPos)) {//Returns if overlapping or not
					//Overlapping the button
					if (entry.*onEvent) {
						(m_observer->*(entry.*onEvent))(entry.name);
					}
					return true;
				} else {
					//Not overlapping - calling the global function
					(entry.c.*evntOtherwise)(cursorPos);
				}
			}
		}
		return false;
	}

	//Non-constant
	template<class Ctrl> void forActiveController(const glm::vec2& cursorPos, void (Ctrl::* function)(const glm::vec2&)) {
		for (auto& entry : std::get<std::vector<MenuEntry<Ctrl, Observer>>>(m_controllers)) {
			if (entry.isActive(m_currentState) && entry.enabled) {
				(entry.c.*function)(cursorPos);
			}
		}
	}
	//Constant
	template<class Ctrl> void cforActiveController(void (Ctrl::* function)() const) const {
		for (auto& entry : std::get<std::vector<MenuEntry<Ctrl, Observer>>>(m_controllers)) {
			if (entry.isActive(m_currentState) && entry.enabled) {
				(entry.c.*function)();
			}
		}
	}
	//Constant, runs on disabled controllers too
	template<class Ctrl> void cforActiveControllerDisabledToo(void (Ctrl::* function)() const) const {
		for (auto& entry : std::get<std::vector<MenuEntry<Ctrl, Observer>>>(m_controllers)) {
			if (entry.isActive(m_currentState)) {
				(entry.c.*function)();
			}
		}
	}



	std::tuple<
		std::vector<MenuEntry<Button, Observer>>,
		std::vector<MenuEntry<TextField, Observer>>,
		std::vector<MenuEntry<Slider, Observer>>,
		std::vector<MenuEntry<PlainText, Observer>>,
		std::vector<MenuEntry<ClosedList, Observer>>> m_controllers;

	std::unordered_map<std::string, MenuIndex> m_names;

	Observer* m_observer = nullptr;

	float m_currentState = 1.0f;

#ifdef RE_MENU_KEEP_INFOS
	std::unordered_map<std::string, ControllerInfo> m_infoMap;
	std::vector<void (Observer::*)(const std::string&)> m_functions;
	RE::SpriteBatch* m_spriteBatch = nullptr;
	RE::TypingHandle m_typingHandle;
#endif
};
}