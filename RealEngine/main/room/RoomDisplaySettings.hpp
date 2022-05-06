/*! 
 *  @author    Dubsky Tomas
 */
#pragma once

#include <glm/vec4.hpp>

namespace RE {

/**
 * @brief Specifies some parameters related to how the room is drawn
*/
struct RoomDisplaySettings {
	glm::vec4 clearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);	/**< The color that is used to clear the window framebuffer */
	unsigned int stepsPerSecond = 50;							/**< The number of steps per second that this romm runs at */
	unsigned int framesPerSecondLimit = 100;					/**< The limit of frames per second that this room will be rendered in */
	bool usingImGui = false;									/**< Tells whether this room uses ImGui. ImGui cannot be used if it is false */
};

}
