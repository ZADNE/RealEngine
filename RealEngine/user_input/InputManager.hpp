/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <unordered_map>

#include <glm/vec2.hpp>

#include <RealEngine/user_input/Key.hpp>

namespace RE {
	
/**
* @brief Keeps track of mouse and keyboard input.
*/
class InputManager {
	friend class MainProgram;
public:
	InputManager();
	~InputManager();

	void update();//Should be called each step

	//Returns for how many steps straight it has been down (0 = is not currently down)
	int isDown(RE::Key keyID) const;
	//Returns how many times it was pressed before this (by how many steps the mouse wheel was rotated)
	int wasPressed(RE::Key keyID) const;
	//Returns usually 0 (was not released) or 1 (was released)
	int wasReleased(RE::Key keyID) const;

	//Setters
	void setCursorAbs(const glm::uvec2& abs);
	//Getters
	glm::uvec2 getCursorAbs() const;
private:
	void press(RE::Key keyID, int times = 1);
	void release(RE::Key keyID);

	int wasDown(RE::Key keyID) const;
	mutable std::unordered_map<RE::Key, int> m_stateMap;
	mutable std::unordered_map<RE::Key, int> m_stateMapPrevious;
	glm::uvec2 m_cursorAbs = glm::uvec2(0u, 0u);

	int m_keysHeld = 0u;
	int m_keysHeldPrevious = 0u;
	Key m_longestHeld = Key::UNKNOWN;
	int m_noKeyHeld = 0u;
	int m_noKeyHeldPrevious = 0u;
};

}