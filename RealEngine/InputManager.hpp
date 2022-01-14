#pragma once
#include <unordered_map>

#include <glm/vec2.hpp>

#include <RealEngine/key.hpp>

namespace RE {

	class InputManager {
		friend class MainProgram;
	public:
		InputManager();
		~InputManager();

		void update();//Should be called each step

			//Returns for how many steps straight it has been down (0 = is not currently down)
		unsigned int isDown(RE::RKey keyID) const;
		//Returns how many times it was pressed before this (by how many steps the mouse wheel was rotated)
		unsigned int wasPressed(RE::RKey keyID) const;
		//Returns usually 0 (was not released) or 1 (was released)
		unsigned int wasReleased(RE::RKey keyID) const;

		//Setters
		void setCursorAbs(const glm::uvec2& abs);
		//Getters
		glm::uvec2 getCursorAbs() const;
	private:
		void press(RE::RKey keyID, int times = 1);
		void release(RE::RKey keyID);

		unsigned int wasDown(RE::RKey keyID) const;
		mutable std::unordered_map<RE::RKey, unsigned int> m_stateMap;
		mutable std::unordered_map<RE::RKey, unsigned int> m_stateMapPrevious;
		glm::uvec2 m_cursorAbs = glm::uvec2(0u, 0u);

		int m_keysHeld = 0u;
		int m_keysHeldPrevious = 0u;
		RKey m_longestHeld = RKey::UNKNOWN;
		unsigned int m_noKeyHeld = 0u;
		unsigned int m_noKeyHeldPrevious = 0u;
	};

}