#pragma once
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include <SDL2/SDL_events.h>

namespace RE {

class View {
public:
	View();
	~View();

	void update();

	void initView(const glm::vec2& viewDimensions);
	void resizeView(const glm::vec2& newDims);

	bool isInsideView(const glm::vec2& position, const glm::vec2& dimension);

	//Cursor
	glm::vec2 convertAbsToRel(const glm::uvec2& abs);
	glm::vec2 getCursorRel();
	void updateCursorRel(const glm::uvec2& absCoord);


	//Setters
	void setPosition(const glm::vec2& newPosition, const glm::uvec2& absCoord) { m_position = newPosition; clip(); m_needsUpdate = true; updateCursorRel(absCoord); };
	void setScale(const glm::vec2& newScale, const glm::uvec2& absCoord) { m_scale = newScale; clip(); m_needsUpdate = true; updateCursorRel(absCoord); };

	//Changers
	void shiftPosition(const glm::vec2& shiftPosition, const glm::uvec2& absCoord) { m_position += shiftPosition; clip(); m_needsUpdate = true; updateCursorRel(absCoord); };
	//Scale is multiplied by zoomVec
	void zoom(const glm::vec2& zoomVec, const glm::uvec2& absCoord) { m_scale *= zoomVec; clip(); m_needsUpdate = true; updateCursorRel(absCoord); };

	//Getters

	//Center of the view
	glm::vec2 getPosition() const { return m_position; };
	//Bottom-left corner of the view
	glm::vec2 getBotLeft() const { return m_position - getScaledDims() / 2.0f; };
	glm::vec2 getScale() const { return m_scale; };
	glm::vec2 getUnscaledDims() const { return m_viewDimensions; }
	glm::vec2 getScaledDims() const { return m_viewDimensions / m_scale; };
	glm::mat4 getViewMatrix() const { return m_viewMatrix; };

	//View clipping
	void enableClipping(const glm::vec2& minXY, const glm::vec2& maxXY);
	void disableClipping();

	//SDL
	void E_SDL(SDL_Event& evnt);

	//Represents main window view (not affected by surfaces)
	static View std;
private:
	bool m_needsUpdate = true;

	glm::vec2 m_position = glm::vec2(0.0f, 0.0f);
	glm::vec2 m_scale = glm::vec2(1.0f, 1.0f);

	glm::mat4 m_viewMatrix = glm::mat4(1.0f);
	glm::mat4 m_orthoMatrix;

	glm::vec2 m_viewDimensions;

	glm::vec2 m_cursorRel = glm::vec2(0.0f, 0.0f);

	//Clipping
	void clip();

	glm::vec2 m_minXY;
	glm::vec2 m_maxXY;
	bool m_clippingEnabled = false;
};

}