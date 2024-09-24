/**
 *  @author    Dubsky Tomas
 */
#include <RealEngine/renderer/DebugMessageHandler.hpp>
#include <RealEngine/utility/Error.hpp>

using enum vk::DebugUtilsMessageSeverityFlagBitsEXT;
using enum vk::DebugUtilsMessageTypeFlagBitsEXT;

namespace re {

namespace {

const char* skipMessageHeader(const char* msg) {
    int skips = 0;
    while (*msg != '\0') {
        bool pipe = (*msg == '|');
        msg++;
        if (pipe) {
            if (++skips == 2)
                break;
        }
    }
    return msg;
}

const char* skipFunctionName(const char* msg) {
    while (*msg != '\0') {
        bool colon = (*msg == ':');
        msg++;
        if (colon) {
            break;
        }
    }
    int spacesSkipped = 0;
    while (*msg != '\0' && spacesSkipped <= 1) {
        if (*msg == ' ') {
            msg++;
            spacesSkipped++;
        } else {
            break;
        }
    }
    return msg;
}

void handleValidationMessage(
    VkDebugUtilsMessageSeverityFlagBitsEXT sev,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData
) {
    std::string str{"##"};
    // Show nested debug labels
    for (int i = 0; i < callbackData->cmdBufLabelCount; i++) {
        str += callbackData->pCmdBufLabels[i].pLabelName;
        if (i < callbackData->cmdBufLabelCount - 1) {
            str += "->";
        }
    }
    str += "\n  ";
    const char* msg = skipMessageHeader(callbackData->pMessage);

    // Report the message
    switch (static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(sev)) {
    case eVerbose:
    case eInfo:
    case eWarning: log(str + msg); break;
    case eError:
    default:       error(str + msg); break;
    }
}

} // namespace

VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallbackHandler(
    VkDebugUtilsMessageSeverityFlagBitsEXT sev,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData
) {
    switch (callbackData->messageIdNumber) {
    case 0x0:        // Ignore loader messages
        break;
    case 0x76589099: // NOLINT(*-magic-numbers): Log debug printf
        log(skipFunctionName(skipMessageHeader(callbackData->pMessage)), false);
        break;
    default: // Log validation message
        handleValidationMessage(sev, type, callbackData, userData);
        break;
    }
    return false;
}
} // namespace re
