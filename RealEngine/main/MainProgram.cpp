#include <RealEngine/main/MainProgram.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

#include <RealEngine/main/Window.hpp>
#include <RealEngine/main/Room.hpp>
#include <RealEngine/main/RoomVector.hpp>
#include <RealEngine/user_input/Key.hpp>
#include <RealEngine/graphics/View.hpp>
#include <RealEngine/graphics/GeometryBatch.hpp>
#include <RealEngine/graphics/SpriteBatch.hpp>
#include <RealEngine/graphics/UniformManager.hpp>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace RE {

	MainProgram* MainProgram::std = nullptr;

	const std::string SETTINGS_FILENAME = "settings.json";
	const std::string SETTINGS_BACKUP_FILENAME = "settings.json - backup";


	MainProgram::MainProgram() {
		p_roomVector = std::make_unique<RoomVector>(this);
		std = this;
	}

	MainProgram::~MainProgram() {

	}

	void MainProgram::runProgram(int argc, char* argv[]) {
		if (!init(argc, argv)) {
			//Failed to initialize
			return;
		}
		p_running = true;
		resumeSteps();

		//MAIN PROGRAM LOOP
		std::cout << "Entering main loop!" << std::endl;
		while (p_running) {
			m_synchronizer.beginFrame();

			//Perform simulation steps to catch up the time
			while (m_synchronizer.shouldStepHappen()) {
				//Check for user input
				if (p_checkForInput) {
					p_inputManager.update();
					checkForSDLEvents();
				}
				else {
					SDL_PumpEvents();
				}
				//Do the step
				step();
			}

			//Draw frame
			draw(m_synchronizer.getDrawInterpolationFactor());

			m_window.swapBuffer();
			glClear(GL_COLOR_BUFFER_BIT);

			m_synchronizer.endFrame();
		}

		//Exiting program
		exitGameAction();
	}

	void MainProgram::exitProgram() {
		p_running = false;
	}

	void MainProgram::exitProgram(int exitcode) {
		p_running = false;
		p_exitCode = exitcode;
	}

	glm::vec2 MainProgram::getWindowDim() const {
		return m_windowDims;
	}

	unsigned int MainProgram::getFramesPerSecond() {
		return m_synchronizer.getFramesPerSecond();
	}

	Synchronizer::Duration MainProgram::getMaxFrameTime() {
		return m_synchronizer.getMaxFrameTime();
	}

	void MainProgram::checkForInput(bool check) {
		p_checkForInput = check;
		p_inputManager.update();
	}

	bool MainProgram::getDisplays(std::vector<RE::DisplayInfo>& infos) const {
		int numberOfDisplays = SDL_GetNumVideoDisplays();
		if (numberOfDisplays < 0) { return false; }
		infos.reserve(numberOfDisplays);
		for (int i = 0; i < numberOfDisplays; ++i) {
			DisplayInfo info;
			info.name = SDL_GetDisplayName(i);
			SDL_Rect rect;
			if (SDL_GetDisplayBounds(i, &rect)) { return false; }
			info.bounds.x = rect.x;
			info.bounds.y = rect.y;
			info.bounds.z = rect.w;
			info.bounds.w = rect.h;
			if (SDL_GetDisplayUsableBounds(i, &rect)) { return false; }
			info.boundsUsable.x = rect.x;
			info.boundsUsable.y = rect.y;
			info.boundsUsable.z = rect.w;
			info.boundsUsable.w = rect.h;
			SDL_DisplayMode mode;
			if (SDL_GetCurrentDisplayMode(i, &mode)) { return false; }
			info.dims.x = mode.w;
			info.dims.y = mode.h;
			info.refreshRate = mode.refresh_rate;
			info.driverSpecific = mode.driverdata;
			info.pixelFormat = mode.format;
			infos.push_back(info);
		}
		return true;
	}

	void MainProgram::step() {
		if (p_currentRoom) {
			switch (p_currentRoom->getState()) {
			case (RoomState::ACTIVE):
				p_currentRoom->E_step();
				break;
			case (RoomState::CHANGE_TO_NEXT):
				pauseSteps();
				p_currentRoom = p_roomVector->gotoNextRoom();
				resumeSteps();
				break;
			case (RoomState::CHANGE_TO_PREV):
				pauseSteps();
				p_currentRoom = p_currentRoom = p_roomVector->gotoPrevRoom();
				resumeSteps();
				break;
			case (RoomState::EXIT_PROGRAM):
				exitProgram();
				break;
			default:
				exitProgram(1);
			}
		}
		else {
			exitProgram(1);
		}
	}

	void MainProgram::draw(double interpolationFactor) {
		p_currentRoom->E_draw(interpolationFactor);
	}

	void MainProgram::exitGameAction() {
		p_currentRoom->E_exit();
		if (p_roomVector) {
			p_roomVector->destroy();
			p_roomVector.reset();
		}
		SDL_Quit();
		if (p_exitCode != 0) {
			exit(p_exitCode);
		}
	}

	void MainProgram::E_SDL(SDL_Event& evnt) {
		RE::Key key = RE::Key::UNKNOWN;
		switch (evnt.type) {
		case SDL_KEYDOWN:
			key = SDLKToREKey(evnt.key.keysym.sym);
			if (evnt.key.repeat == 0 && !(p_typeString && p_blockPressInput)) {
				p_inputManager.press(RE::Key::ANY_KEY);
				p_inputManager.press(key);
			}
			if ((key == RE::Key::Backspace) && p_typeString) {
				//Used backspace when typing - removing last character
				if (p_typeString->size() >= 1) {
					std::cerr << "TODO\n";
					/*size_t size = m_convert_utf8_utf16.to_bytes(m_convert_utf8_utf16.from_bytes(*p_typeString).back()).size();
					for (size_t i = 0u; i < size; ++i) {
						p_typeString->pop_back();
					}*/
				}
			}
			break;
		case SDL_KEYUP:
			if (evnt.key.repeat == 0) {
				p_inputManager.press(RE::Key::ANY_KEY, -1);
				p_inputManager.release(SDLKToREKey(evnt.key.keysym.sym));
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (evnt.key.repeat == 0) {
				auto key = SDLKToREKey(evnt.button.button);
				p_inputManager.press(RE::Key::ANY_KEY);
				p_inputManager.press(key, evnt.button.clicks);
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (evnt.key.repeat == 0) {
				p_inputManager.press(RE::Key::ANY_KEY, -1);
				p_inputManager.release(SDLKToREKey(evnt.button.button));
			}
			break;
		case SDL_MOUSEMOTION:
			p_inputManager.setCursorAbs(glm::uvec2(evnt.motion.x, m_window.getDims().y - evnt.motion.y - 1));//Reversing Y coordinate to get standard math coordinates
			break;
		case SDL_MOUSEWHEEL:
			key = (evnt.wheel.y > 0) ? (Key::UMW) : (Key::DMW);
			p_inputManager.press(key, std::abs(evnt.wheel.y));

			key = (evnt.wheel.x > 0) ? (Key::RMW) : (Key::LMW);
			p_inputManager.press(key, std::abs(evnt.wheel.x));
			break;
		case SDL_TEXTINPUT:
			*p_typeString += evnt.text.text;
			break;
		case SDL_QUIT:
			exitProgram();
			break;
		}
	}

	bool MainProgram::goToRoom(int index) {
		pauseSteps();
		p_currentRoom = p_roomVector->gotoRoom(index);
		resumeSteps();
		return false;
	}

	void MainProgram::setStepsPerSecond(unsigned int stepsPerSecond) {
		m_synchronizer.setStepsPerSecond(stepsPerSecond);
	}

	void MainProgram::setFramesPerSecondLimit(unsigned int framesPerSecondLimit) {
		m_synchronizer.setFramesPerSecondLimit(framesPerSecondLimit);
	}

	void MainProgram::pauseSteps() {
		m_synchronizer.pauseSteps();
	}

	void MainProgram::resumeSteps() {
		m_synchronizer.resumeSteps();
	}

	void MainProgram::resizeWindow(const glm::ivec2& newDims, bool isPermanent) {//isPermanent indicates whether the new dimensions should be saved into settings (and thus be default next time the game is launched)
		m_window.resizeWindow(newDims);
		glViewport(0, 0, (GLsizei)newDims.x, (GLsizei)newDims.y);
		//Updating standard objects
		//View
		View::std.resizeView((glm::vec2)newDims);
		View::std.setPosition((glm::vec2)newDims / 2.0f, glm::uvec2(0));
		View::std.update();
		//Uniforms
		glm::mat4 matrix = View::std.getViewMatrix();
		UniformManager::std.setUniformBuffer("GlobalMatrices", 0u, sizeof(glm::mat4), &matrix[0][0]);
		m_windowDims = (glm::vec2)newDims;

		if (isPermanent) {
			saveSettings();
		}
	}

	void MainProgram::goFullscreen(bool fullscreen, bool isPermanent) {
		m_windowFlags.fullscreen = fullscreen;
		m_window.goFullscreen(fullscreen);
		if (isPermanent) {
			saveSettings();
		}
	}

	void MainProgram::goBorderless(bool borderless, bool isPermanent) {
		m_windowFlags.borderless = borderless;
		m_window.goBorderless(borderless);
		if (isPermanent) {
			saveSettings();
		}
	}

	void MainProgram::setVSync(bool vSync, bool isPermanent) {
		m_windowFlags.vSync = vSync;
		m_window.setVSync(vSync);
		if (isPermanent) {
			saveSettings();
		}
	}

	void MainProgram::setWindowTitle(const std::string& title) {
		m_window.setTitle(title);
	}

	std::string MainProgram::getWindowTitle() const {
		return m_window.getTitle();
	}

	void MainProgram::setTypeString(FontString* string, bool blockPressInput/* = false*/) {
		p_typeString = string;
		p_blockPressInput = blockPressInput;
		if (p_typeString) {
			SDL_StartTextInput();
		}
		else {
			SDL_StopTextInput();
		}
	}

	FontString const* MainProgram::getTypeString() const {
		return p_typeString;
	}

	bool MainProgram::init(int argc, char* argv[]) {
		RE::initRE();

		if (!initSystems()) {
			return false;
		}

		std::vector<std::string> cmdLnArg;
		cmdLnArg.reserve(argc);
		for (int i = 0; i < argc; ++i) {
			cmdLnArg.push_back(argv[i]);
		}

		E_Init(cmdLnArg);

		p_currentRoom = p_roomVector->getCurrentRoom();

		return true;
	}

	bool MainProgram::initSystems() {
		std::ifstream i(SETTINGS_FILENAME);
		json j;
		try {
			i >> j;
			m_windowDims.x = j["window"]["width"].get<float>();
			m_windowDims.y = j["window"]["height"].get<float>();
			m_windowFlags.fullscreen = j["window"]["fullscreen"].get<bool>();
			m_windowFlags.borderless = j["window"]["borderless"].get<bool>();
			m_windowFlags.vSync = j["window"]["vsync"].get<bool>();
			i.close();
		}
		catch (...) {
			printf("Settings either don't exist or are currupted, backing up these and resetting settings.\n");
			i.close();
			if (std::filesystem::exists(SETTINGS_FILENAME)) {
				std::filesystem::rename(SETTINGS_FILENAME, SETTINGS_BACKUP_FILENAME);
			}
			resetSettings();
			saveSettings();
		}

		std::string defTitle = "RealEngine v" + std::to_string(RE_VERSION_MAJOR) + "." + std::to_string(RE_VERSION_MINOR) + "." + std::to_string(RE_VERSION_PATCH);
		m_window.createWindow(defTitle, (glm::ivec2)m_windowDims, m_windowFlags);

		glEnable(GL_PRIMITIVE_RESTART);
		glPrimitiveRestartIndex(PRIMITIVE_RESTART_INDEX);

		//Initialize standard view
		View::std.initView(m_windowDims);
		View::std.setPosition(m_windowDims / 2.0f, glm::uvec2(0));
		View::std.update();

		//Standard shaders
		m_stdSpriteShader = RE::RM::getShaderProgram(ShaderProgramSource::stdSprite);
		m_stdGeometryShader = RE::RM::getShaderProgram(ShaderProgramSource::stdGeometry);
		//Standard uniforms
		UniformManager::std.addUniformBuffer("GlobalMatrices", sizeof(glm::mat4));
		UniformManager::std.addShader("GlobalMatrices", m_stdSpriteShader.get());
		UniformManager::std.addShader("GlobalMatrices", m_stdGeometryShader.get());
		glm::mat4 matrix = View::std.getViewMatrix();
		UniformManager::std.setUniformBuffer("GlobalMatrices", 0u, sizeof(glm::mat4), &matrix[0][0]);

		return true;
	}

	void MainProgram::resetSettings() {
		m_windowDims = glm::vec2(1280.0f, 1000.0f);
		m_windowFlags = WindowFlags{};
	}

	void MainProgram::saveSettings() {
		std::cout << "Saving settings.\n";

		nlohmann::ordered_json j = {
			{"window", {
				{"width", (unsigned int)m_windowDims.x},
				{"height", (unsigned int)m_windowDims.y},
				{"fullscreen", (bool)m_windowFlags.fullscreen},
				{"borderless", (bool)m_windowFlags.borderless},
				{"vsync", (bool)m_windowFlags.vSync}
			}}
		};

		std::ofstream o(SETTINGS_FILENAME, std::ofstream::trunc);
		o << j.dump(2);
		o.close();
	}

	void MainProgram::checkForSDLEvents() {
		SDL_Event evnt;
		while (SDL_PollEvent(&evnt)) {
			E_SDL(evnt);
		}
	}
}