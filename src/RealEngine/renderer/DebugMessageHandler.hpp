/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vulkan/vulkan.hpp>

namespace re {

/**
 * @brief Used by VulkanFixture to process Vulkan messages
 */
VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallbackHandler(
    VkDebugUtilsMessageSeverityFlagBitsEXT sev,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData
);

} // namespace re
