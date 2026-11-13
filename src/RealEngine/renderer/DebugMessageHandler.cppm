/**
 *  @author    Dubsky Tomas
 */
module;
#include <vulkan/vulkan.h>

export module realengine:renderer.DebugMessageHandler;
import vulkan;

export namespace re {

/**
 * @brief Is used by VulkanRenderer to process Vulkan messages
 */
VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallbackHandler(
    vk::DebugUtilsMessageSeverityFlagBitsEXT sev,
    vk::DebugUtilsMessageTypeFlagsEXT type,
    const vk::DebugUtilsMessengerCallbackDataEXT* callbackData, void* userData
);

} // namespace re
