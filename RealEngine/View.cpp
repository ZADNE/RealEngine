#include <RealEngine/View.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <RealEngine/utility.hpp>

#include <RealEngine/InputManager.hpp>

namespace RE {

	View View::std = View{};

	View::View() {

	}

	View::~View() {

	}

	void View::initView(const glm::vec2& viewDimensions) {
		m_viewDimensions = viewDimensions;
		m_orthoMatrix = glm::ortho(0.0f, m_viewDimensions.x, 0.0f, m_viewDimensions.y);
	}

	void View::resizeView(const glm::vec2& newDims) {
		m_viewDimensions = newDims;
		m_orthoMatrix = glm::ortho(0.0f, newDims.x, 0.0f, newDims.y);
		m_needsUpdate = true;
		update();
	}

	bool View::isInsideView(const glm::vec2& position, const glm::vec2& dimension) {
		//position MUST BE center of the box

		//Maximum deviation to be inside the view
		glm::vec2 scaledMaxDeviation = (m_viewDimensions + dimension) / 2.0f * m_scale;
		glm::vec2 distanceBetween = abs(glm::vec2(position.x, position.y) - m_position);

		if (distanceBetween.x <= scaledMaxDeviation.x && distanceBetween.y <= scaledMaxDeviation.y) {
			return true;
		}

		return false;
	}

	glm::vec2 View::convertAbsToRel(const glm::uvec2& abs) {
		//Center [0;0]
		glm::vec2 floatAbs = glm::vec2(abs);
		floatAbs -= m_viewDimensions / 2.0f;
		//Scaling
		floatAbs /= m_scale;
		//Translation
		floatAbs += m_position;
		return (glm::vec2)floatAbs;
	}

	void View::update() {//Should be called once every step before drawing
		if (m_needsUpdate) {
			//Translate
			glm::vec3 translate(-m_position.x + m_viewDimensions.x / 2.0f, -m_position.y + m_viewDimensions.y / 2.0f, 0.0f);
			m_viewMatrix = glm::translate(m_orthoMatrix, translate);
			//Scale
			glm::vec3 scale(m_scale.x, m_scale.y, 0.0f);
			m_viewMatrix = glm::scale(glm::mat4(1.0f), scale) * m_viewMatrix;
			m_needsUpdate = false;
		}
	}

	glm::vec2 View::getCursorRel() {
		return m_cursorRel;
	}

	void View::updateCursorRel(const glm::uvec2& absCoord) {
		m_cursorRel = convertAbsToRel(absCoord);
	}

	void View::enableClipping(const glm::vec2& minXY, const glm::vec2& maxXY) {
		m_minXY = minXY;
		m_maxXY = maxXY;
		m_clippingEnabled = true;
		m_needsUpdate = true;
	}

	void View::disableClipping() {
		m_clippingEnabled = false;
	}

	void View::E_SDL(SDL_Event& evnt) {
		switch (evnt.type) {
		case SDL_MOUSEMOTION:
			updateCursorRel(glm::uvec2(evnt.motion.x, evnt.motion.y));
			break;
		}
	}
	void View::clip() {
		if (!m_clippingEnabled) { return; }
		m_position.x = rmath::clamp(m_position.x, m_minXY.x + m_viewDimensions.x / 2.0f / m_scale.x, m_maxXY.x - m_viewDimensions.x / 2.0f / m_scale.x);
		m_position.y = rmath::clamp(m_position.y, m_minXY.y + m_viewDimensions.y / 2.0f / m_scale.y, m_maxXY.y - m_viewDimensions.y / 2.0f / m_scale.y);
	}
}