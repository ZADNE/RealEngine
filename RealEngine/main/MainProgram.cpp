#include <RealEngine/main/MainProgram.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

#include <SDL2/SDL_events.h>

#include <RealEngine/resources/ResourceManager.hpp>
#include <RealEngine/graphics/SpriteBatch.hpp>
#include <RealEngine/graphics/GeometryBatch.hpp>
#include <RealEngine/graphics/default_shaders.hpp>
#include <RealEngine/graphics/Viewport.hpp>

namespace RE {

MainProgram* MainProgram::std = nullptr;

int MainProgram::run() {
	doRoomTransitionIfScheduled();
	if (!p_roomManager.getCurrentRoom()) {
		throw std::runtime_error("Initial room was not set");
	}

	m_programShouldRun = true;
	p_synchronizer.resumeSteps();

	//MAIN PROGRAM LOOP
	std::cout << "Entering main loop!" << std::endl;
	while (m_programShouldRun) {
		p_synchronizer.beginFrame();
		glClear(GL_COLOR_BUFFER_BIT);

		//Perform simulation steps to catch up the time
		while (p_synchronizer.shouldStepHappen()) {
			//Check for user input
			if (m_checkForInput) {
				p_inputManager.update();
				checkForSDLEvents();
			} else {
				SDL_PumpEvents();
			}
			//Do the simulation step
			step();
		}

		//Draw frame
		render(p_synchronizer.getDrawInterpolationFactor());

		p_window.swapBuffer();

		doRoomTransitionIfScheduled();

		p_synchronizer.endFrame();
	}

	//Exit the program
	p_roomManager.getCurrentRoom()->sessionEnd();

	return m_programExitCode;
}

void MainProgram::scheduleProgramExit(int exitcode/* = EXIT_SUCCESS*/) {
	m_programShouldRun = false;
	m_programExitCode = exitcode;
}

void MainProgram::checkForInput(bool check) {
	m_checkForInput = check;
	p_inputManager.update();
}

std::vector<RE::DisplayInfo> MainProgram::getDisplays() const {
	std::vector<RE::DisplayInfo> infos;
	int numberOfDisplays = SDL_GetNumVideoDisplays();
	if (numberOfDisplays < 0) { return infos; }
	infos.reserve(numberOfDisplays);
	for (int i = 0; i < numberOfDisplays; ++i) {
		DisplayInfo info;
		info.name = SDL_GetDisplayName(i);
		SDL_Rect rect;
		if (SDL_GetDisplayBounds(i, &rect)) { continue; }
		info.bounds.x = rect.x;
		info.bounds.y = rect.y;
		info.bounds.z = rect.w;
		info.bounds.w = rect.h;
		if (SDL_GetDisplayUsableBounds(i, &rect)) { continue; }
		info.boundsUsable.x = rect.x;
		info.boundsUsable.y = rect.y;
		info.boundsUsable.z = rect.w;
		info.boundsUsable.w = rect.h;
		SDL_DisplayMode mode;
		if (SDL_GetCurrentDisplayMode(i, &mode)) { continue; }
		info.dims.x = mode.w;
		info.dims.y = mode.h;
		info.refreshRate = mode.refresh_rate;
		info.driverSpecific = mode.driverdata;
		info.pixelFormat = mode.format;
		infos.push_back(info);
	}
	return infos;
}

void MainProgram::step() {
	p_roomManager.getCurrentRoom()->step();
}

void MainProgram::render(double interpolationFactor) {
	p_roomManager.getCurrentRoom()->render(interpolationFactor);
}

void MainProgram::E_SDL(SDL_Event* evnt) {
	RE::Key key = RE::Key::UNKNOWN;
	switch (evnt->type) {
	case SDL_KEYDOWN:
		key = SDLKToREKey(evnt->key.keysym.sym);
		if (evnt->key.repeat == 0 && !(m_typeString && m_blockPressInput)) {
			p_inputManager.press(RE::Key::ANY_KEY);
			p_inputManager.press(key);
		}
		if ((key == RE::Key::Backspace) && m_typeString) {
			//Used backspace when typing - removing last character
			if (m_typeString->size() >= 1) {
				std::cerr << "TODO\n";
				/*size_t size = m_convert_utf8_utf16.to_bytes(m_convert_utf8_utf16.from_bytes(*p_typeString).back()).size();
				for (size_t i = 0u; i < size; ++i) {
					p_typeString->pop_back();
				}*/
			}
		}
		break;
	case SDL_KEYUP:
		if (evnt->key.repeat == 0) {
			p_inputManager.press(RE::Key::ANY_KEY, -1);
			p_inputManager.release(SDLKToREKey(evnt->key.keysym.sym));
		}
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (evnt->key.repeat == 0) {
			auto key = SDLKToREKey(evnt->button.button);
			p_inputManager.press(RE::Key::ANY_KEY);
			p_inputManager.press(key, evnt->button.clicks);
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if (evnt->key.repeat == 0) {
			p_inputManager.press(RE::Key::ANY_KEY, -1);
			p_inputManager.release(SDLKToREKey(evnt->button.button));
		}
		break;
	case SDL_MOUSEMOTION:
		p_inputManager.setCursorAbs(glm::uvec2(evnt->motion.x, p_window.getDims().y - evnt->motion.y - 1));//Reversing Y coordinate to get standard math coordinates
		break;
	case SDL_MOUSEWHEEL:
		key = (evnt->wheel.y > 0) ? (Key::UMW) : (Key::DMW);
		p_inputManager.press(key, std::abs(evnt->wheel.y));

		key = (evnt->wheel.x > 0) ? (Key::RMW) : (Key::LMW);
		p_inputManager.press(key, std::abs(evnt->wheel.x));
		break;
	case SDL_TEXTINPUT:
		*m_typeString += evnt->text.text;
		break;
	case SDL_QUIT:
		scheduleProgramExit();
		break;
	}
}

void MainProgram::doRoomTransitionIfScheduled() {
	if (m_nextRoomIndex == NO_NEXT_ROOM) return;
	p_synchronizer.pauseSteps();
	auto prev = p_roomManager.getCurrentRoom();
	auto current = p_roomManager.gotoRoom(m_nextRoomIndex, m_roomTransitionParameters);
	bool chaged = (prev != current);
	if (chaged) {
		step();
	}
	m_nextRoomIndex = NO_NEXT_ROOM;
	p_synchronizer.resumeSteps();
}

void MainProgram::scheduleRoomTransition(size_t index, RoomTransitionParameters params) {
	m_nextRoomIndex = index;
	m_roomTransitionParameters = params;
}

void MainProgram::setTypeString(FontString* string, bool blockPressInput/* = false*/) {
	m_typeString = string;
	m_blockPressInput = blockPressInput;
	if (m_typeString) {
		SDL_StartTextInput();
	} else {
		SDL_StopTextInput();
	}
}

FontString const* MainProgram::getTypeString() const {
	return m_typeString;
}

void MainProgram::checkForSDLEvents() {
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		E_SDL(&evnt);
	}
}


MainProgram::MainProgram() {
	std = this;
	Room::m_mainProgram = this;
	Room::m_inputManager = &p_inputManager;
	Room::m_synchronizer = &p_synchronizer;
	Room::m_window = &p_window;

	auto spriteShader = RE::RM::getShaderProgram({.vert = sprite_vert, .frag = sprite_frag});
	Viewport::getWindowMatrixUniformBuffer().connectToShaderProgram(*spriteShader, 0u);
	SpriteBatch::std().switchShaderProgram(spriteShader);

	auto geometryShader = RE::RM::getShaderProgram({.vert = geometry_vetr, .frag = geometry_frag});
	Viewport::getWindowMatrixUniformBuffer().connectToShaderProgram(*geometryShader, 0u);
	GeometryBatch::std().switchShaderProgram(geometryShader);
}

MainProgram::~MainProgram() {

}

}