/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/renderer/VulkanObject.hpp>

namespace re {

/**
 * @brief Specifies parameters for RenderPass creation
 */
struct RenderPassCreateInfo {
    std::span<const vk::AttachmentDescription2> attachments;
    std::span<const vk::SubpassDescription2>    subpasses;
    std::span<const vk::SubpassDependency2>     dependencies;

    // Debug
    [[no_unique_address]] DebugName<> debugName;
};

/**
 * @brief Identifies a subpass within a renderpass
 * @note This does not hold ownership of the renderpass!
 */
struct RenderPassSubpass {
    vk::RenderPass renderPass{};
    uint32_t       subpassIndex{};
};

/**
 * @brief Represents a collection of attachments, subpasses, and dependencies
 * between the subpasses, and describes how the attachments are used over the
 * course of the subpasses.
 */
class RenderPass: public VulkanObject {
public:
    /**
     * @brief Constructs a null renderpass that cannot be used for rendering
     */
    explicit RenderPass() {}

    /**
     * @brief Constructs a renderpass that can be used for rendering
     */
    explicit RenderPass(const RenderPassCreateInfo& createInfo);

    RenderPass(const RenderPass&)            = delete; /**< Noncopyable */
    RenderPass& operator=(const RenderPass&) = delete; /**< Noncopyable */

    RenderPass(RenderPass&& other) noexcept;            /**< Movable */
    RenderPass& operator=(RenderPass&& other) noexcept; /**< Movable */

    ~RenderPass();

    RenderPassSubpass subpass(uint32_t index) const {
        return RenderPassSubpass{m_renderPass, index};
    }

    const vk::RenderPass& operator*() const { return m_renderPass; }
    const vk::RenderPass* operator->() const { return &m_renderPass; }

    const vk::RenderPass& renderPass() const { return m_renderPass; }

private:
    vk::RenderPass m_renderPass{};
};

} // namespace re
