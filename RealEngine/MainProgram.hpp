#pragma once
#include <memory>
#include <codecvt>

#include <SDL\SDL_events.h>
#include <glm\vec2.hpp>

#include <RealEngine/RoomVector.hpp>
#include <RealEngine/RealEngine.hpp>
#include <RealEngine/InputManager.hpp>
#include <RealEngine/Window.hpp>
#include <RealEngine/Synchronizer.hpp>
#include <RealEngine/ResourceManager.hpp>


namespace RE {

	class RoomVector;
	class Room;
	struct DisplayInfo {
		std::string name; //UTF-8
		glm::ivec4 bounds; //In pixels; XYWH
		glm::ivec4 boundsUsable; //In pixels; XYWH; system-reserved parts removed
		glm::ivec2 dims; //In pixels
		int refreshRate; //In Hz, 0 = unknown
		Uint32 pixelFormat; //See SDL2's SDL_PixelFormatEnum
		void* driverSpecific;
	};

	/** @class MainProgram
	* @brief This is the centre point of the whole application.
	*
	* This class encapsulates many functionalities.
	* It creates main of window, keeps track of rooms and allows transitions among them,
	* keeps physics steps running at constant frequency (and allows limiting of FPS),
	* makes user input available (mouse, keyboard, OS), and much more...
	*/
	class MainProgram {
	public:
		//Automatically targets MainProgram::std to this
		MainProgram();
		virtual ~MainProgram();

		void runProgram(int argc, char* argv[]);

		//Program ends at the end of the current step
		void exitProgram();
		void exitProgram(int exitcode);

		virtual void E_Init(const std::vector<std::string>& cmdLnArg) = 0;
		virtual void E_Exit() = 0;

		bool goToRoom(int index);

		void setStepsPerSecond(unsigned int stepsPerSecond);/**< @copydoc Synchronizer::setStepsPerSecond */
		void setFramesPerSecondLimit(unsigned int framesPerSecondLimit);/**< @copydoc Synchronizer::setFramesPerSecondLimit */
		unsigned int getFramesPerSecond();/**< @copydoc Synchronizer::getFramesPerSecond */
		Synchronizer::Duration getMaxFrameTime();/**< @copydoc Synchronizer::getMaxFrameTime */
		void pauseSteps();/**< @copydoc Synchronizer::pauseSteps */
		void resumeSteps();/**< @copydoc Synchronizer::resumeSteps */

		//Window related
		void resizeWindow(const glm::ivec2& newDims, bool isPermanent);
		//Resizes window to fullscreen if true
		void goFullscreen(bool fullscreen, bool isPermanent);
		void goBorderless(bool borderless, bool isPermanent);
		void setVSync(bool vSync, bool isPermanent);
		void setWindowTitle(const std::string& title);
		std::string getWindowTitle() const;
		glm::vec2 getWindowDim() const;

		//The string will be edited as the player types
		//Use nullptr to stop typing
		void setTypeString(std::string* string, bool blockPressInput = false);
		//Returns the current target for typing = nullptr means there is not target
		std::string const* getTypeString() const;

		//Getters
		InputManager* IM() {
			return &p_inputManager;
		};


		void checkForInput(bool check);

		static MainProgram* std;

		//Gets Info about all displays
		//Retuns true on success, false on failure
		bool getDisplays(std::vector<RE::DisplayInfo>& infos) const;
	protected:
		void step();
		void draw(double interpolationFactor);
		void exitGameAction();

		bool init(int argc, char* argv[]);
		bool initSystems();


		void checkForSDLEvents();
		void E_SDL(SDL_Event& evnt);

		std::unique_ptr<RoomVector> p_roomVector = nullptr;
		Room* p_currentRoom = nullptr;
		bool p_running = false;
		int p_exitCode = 0;
		InputManager p_inputManager;

		bool p_checkForInput = true;

		//Typing
		std::string* p_typeString = nullptr;
		bool p_blockPressInput = false;

	private:
		static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> m_convert_utf8_utf16;

		Window m_window;

		Synchronizer m_synchronizer{ 50u, 50u };

		glm::vec2 m_windowDims;
		WindowFlags m_windowFlags;

		//Settings related
		void resetSettings();
		//Saves current settings
		void saveSettings();

		ShaderProgramPtr m_stdSpriteShader;
		ShaderProgramPtr m_stdGeometryShader;
	};

}
