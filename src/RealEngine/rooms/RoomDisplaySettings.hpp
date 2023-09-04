/*!
 *  @author    Dubsky Tomas
 */
#pragma once

#include <glm/vec4.hpp>

namespace re {

/**
 * @brief Specifies parameters that determine how the room is drawn
 */
struct RoomDisplaySettings {
    /**
     * @brief Is used to clear the window framebuffer
     */
    glm::vec4 clearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    /**
     * @brief The number of steps per second that this romm runs at
     */
    unsigned int stepsPerSecond = 50;
    /**
     * @brief The limit of frames per second that this room will be rendered in
     */
    unsigned int framesPerSecondLimit = 100;
    /**
     * @brief Tells whether this room uses ImGui.
     * @warning ImGui may not be used if it is false
     */
    bool usingImGui = false; /**<  */
};

} // namespace re
