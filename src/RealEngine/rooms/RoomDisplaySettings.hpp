/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <span>

#include <vulkan/vulkan.hpp>

namespace re {

/**
 * @brief Specifies parameters that determine how the room is drawn
 */
struct RoomDisplaySettings {

    constexpr static vk::ClearValue k_defaultClearColor =
        vk::ClearColorValue{1.0f, 1.0f, 1.0f, 1.0f};

    /**
     * @brief Values that are used to clear render outputs
     */
    std::span<const vk::ClearValue> clearValues{&k_defaultClearColor, 1};
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
    bool usingImGui = false;
};

} // namespace re
