/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vulkan/vulkan.hpp>

namespace re {

/**
 * @brief Identifies a subpass within a renderpass
 * @note This does not hold ownership of the renderpass!
 */
struct RenderPassSubpass {
    vk::RenderPass renderPass{};
    uint32_t subpassIndex{};
};

} // namespace re
