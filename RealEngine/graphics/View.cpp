﻿/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/View.hpp>

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include <RealEngine/user_input/InputManager.hpp>

namespace RE {

View::View(const glm::vec2& viewDimensions) {
	m_viewDimensions = viewDimensions;
	m_orthoMatrix = glm::ortho(0.0f, m_viewDimensions.x, 0.0f, m_viewDimensions.y);
	update();
}

View::~View() {

}

void View::resizeView(const glm::vec2& newDims) {
	m_viewDimensions = newDims;
	m_orthoMatrix = glm::ortho(0.0f, newDims.x, 0.0f, newDims.y);
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

void View::setCursorAbs(const glm::vec2& cursorAbs) {
	m_cursorAbs = cursorAbs;
	m_cursorRel = convertAbsToRel(cursorAbs);
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

void View::update() {
	//Clip
	clip();
	//Translate
	glm::vec3 translate(-m_position.x + m_viewDimensions.x / 2.0f, -m_position.y + m_viewDimensions.y / 2.0f, 0.0f);
	m_viewMatrix = glm::translate(m_orthoMatrix, translate);
	//Scale
	glm::vec3 scale(m_scale.x, m_scale.y, 0.0f);
	m_viewMatrix = glm::scale(glm::mat4(1.0f), scale) * m_viewMatrix;
	//Update relative cursor position
	m_cursorRel = convertAbsToRel(m_cursorAbs);
}

glm::vec2 View::getCursorRel() {
	return m_cursorRel;
}

void View::enableClipping(const glm::vec2& minXY, const glm::vec2& maxXY) {
	m_minXY = minXY;
	m_maxXY = maxXY;
	m_clippingEnabled = true;
}

void View::disableClipping() {
	m_clippingEnabled = false;
}

void View::clip() {
	if (!m_clippingEnabled) { return; }
	m_position.x = std::clamp(m_position.x, m_minXY.x + m_viewDimensions.x / 2.0f / m_scale.x, m_maxXY.x - m_viewDimensions.x / 2.0f / m_scale.x);
	m_position.y = std::clamp(m_position.y, m_minXY.y + m_viewDimensions.y / 2.0f / m_scale.y, m_maxXY.y - m_viewDimensions.y / 2.0f / m_scale.y);
}

}