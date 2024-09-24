/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <memory>
#include <span>
#include <string_view>

#include <vulkan/vulkan.hpp>

namespace re {

/**
 * @brief The extensions that must be supported on a physical device to be suitable.
 */
constexpr std::array k_deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

/**
 * @brief Tells which device was selected
 */
struct SelectedPhysDevice {
    vk::PhysicalDevice selected{};
    uint32_t graphicsCompQueueFamIndex{};
    uint32_t presentationQueueFamIndex{};

    operator bool() const { return selected != nullptr; }
};

/**
 * @brief Specifies what the selected device must support
 */
struct PhysDeviceRequirements {
    vk::SurfaceKHR surface;              ///< Must support k_surfaceFormat
    const void* deviceCreateInfoChain{}; ///< Must support all features in chain
    std::string_view preferredDevice;    ///< Takes precedence if it is suitable
};

/**
 * @brief Selects the device that meets all requirements
 */
SelectedPhysDevice selectSuitablePhysDevice(
    vk::Instance instance, const PhysDeviceRequirements& req
);

} // namespace re
