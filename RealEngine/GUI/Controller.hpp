#pragma once
#include <glm/vec2.hpp>

namespace RGUI {

	const float SUBIMAGE_CORNER = 0.0f;
	const float SUBIMAGE_EDGE = 1.0f;
	const float SUBIMAGE_FILLING = 2.0f;
	const float SUBIMAGE_MIDDLE_CORNER = 3.0f;
	const float SUBIMAGE_SEPARATOR = 4.0f;

	const float STATE_NORMAL = 0.0f;
	const float STATE_HIGHLIGHTED = 1.0f;
	const float STATE_USED = 2.0f;
	const float STATE_DISABLED = 3.0f;
	const float STATE_BAD_INPUT = 4.0f;

	class Controller {
	public:
		virtual bool overlaps(const glm::vec2& cursorPos) const = 0;

		float getState() const { return p_state; }
		void setState(float newState) { p_state = newState; }

	protected:
		float p_state = STATE_NORMAL;
	};

}