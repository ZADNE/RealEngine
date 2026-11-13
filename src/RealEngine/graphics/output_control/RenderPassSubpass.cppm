/**
 *  @author    Dubsky Tomas
 */
export module realengine:graphics.output_control.RenderPassSubpass;
import vulkan;

export namespace re {

/**
 * @brief Identifies a subpass within a renderpass
 * @note This does not hold ownership of the renderpass!
 */
struct RenderPassSubpass {
    vk::RenderPass renderPass{};
    std::uint32_t subpassIndex{};
};

} // namespace re
