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

	/**
	 * @brief Should be called each step by the MainProgram
	*/
	void update();

	/**
	 * @brief Checks whether the key is currently pressed
	 * @return For how many steps straight it has been down (0 = is not currently down)
	*/
	int isDown(RE::Key keyID) const;

	/**
	 * @brief Checks whther the key has just been pressed (= right before this step)
	 * @return	How many times it has been pressed before this step (by how many steps the mouse wheel was rotated)
	 *			Usually returns 0 or 1.
	*/
	int wasPressed(RE::Key keyID) const;

	/**
	 * @brief Checks whther the key has just been released (= right before this step)
	 * @return 1 if the key has just been released, 0 otherwise
	*/
	int wasReleased(RE::Key keyID) const;

	/**
	 * @brief Gets absolute position of the cursor.
	*/
	glm::ivec2 getCursorAbs() const;

	/**
	 * @brief Gets position of the cursor relative to previous step.
	*/
	glm::ivec2 getCursorRel() const;

	//Setters
	void setCursor(const glm::ivec2& abs, const glm::ivec2& rel);

private:

	void press(RE::Key keyID, int times = 1);
	void release(RE::Key keyID);

	int wasDown(RE::Key keyID) const;
	mutable std::unordered_map<RE::Key, int> m_stateMap;
	mutable std::unordered_map<RE::Key, int> m_stateMapPrevious;
	glm::ivec2 m_cursorAbs = glm::ivec2(0u, 0u);
	glm::ivec2 m_cursorRel = glm::ivec2(0u, 0u);

	int m_keysHeld = 0u;
	int m_keysHeldPrevious = 0u;
	Key m_longestHeld = Key::UNKNOWN;
	int m_noKeyHeld = 0u;
	int m_noKeyHeldPrevious = 0u;
};

}