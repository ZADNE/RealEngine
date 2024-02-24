/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <span>

#include <RealEngine/graphics/output_control/RenderPass.hpp>

namespace re {

/**
 * @brief Surface format that is used for default surface
 */
constexpr vk::SurfaceFormatKHR k_surfaceFormat{
    vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear
};

namespace default_renderpass {

constexpr vk::AttachmentDescription2 k_attachmentDescription{
    {},
    k_surfaceFormat.format,
    vk::SampleCountFlagBits::e1,
    vk::AttachmentLoadOp::eClear,     // Color
    vk::AttachmentStoreOp::eStore,    // Color
    vk::AttachmentLoadOp::eDontCare,  // Stencil
    vk::AttachmentStoreOp::eDontCare, // Stencil
    vk::ImageLayout::eUndefined,      // Initial
    vk::ImageLayout::ePresentSrcKHR   // Final
};
constexpr vk::AttachmentReference2 k_attachmentRef{
    0, vk::ImageLayout::eColorAttachmentOptimal
};
constexpr vk::SubpassDescription2 k_subpassDescription{
    {},
    vk::PipelineBindPoint::eGraphics,
    0u,
    0u,
    nullptr, // Input attachments
    1u,
    &k_attachmentRef // Color attachments
};
constexpr vk::SubpassDependency2 k_subpassDependency{
    vk::SubpassExternal,                                 // Src subpass
    0u,                                                  // Dst subpass
    {vk::PipelineStageFlagBits::eColorAttachmentOutput}, // Src stage mask
    {vk::PipelineStageFlagBits::eColorAttachmentOutput}, // Dst stage mask
    vk::AccessFlags{},                                   // Src access mask
    {vk::AccessFlagBits::eColorAttachmentWrite}          // Dst access mask
};

constexpr RenderPassCreateInfo k_createInfo{
    .attachments  = {&k_attachmentDescription, 1},
    .subpasses    = {&k_subpassDescription, 1},
    .dependencies = {&k_subpassDependency, 1},
    .debugName    = "re::default_renderpass"
};

} // namespace default_renderpass

/**
 * @brief Specifies parameters that determine how the room is drawn
 */
struct RoomDisplaySettings {
    /**
     * @brief The number of steps per second that this romm runs at
     */
    unsigned int stepsPerSecond = 50;
    /**
     * @brief The limit of frames per second that this room will be rendered in
     */
    unsigned int framesPerSecondLimit = 100;
    /**
     * @brief Is the create info used to construct the main renderpass of the room
     */
    const RenderPassCreateInfo* mainRenderPass = &default_renderpass::k_createInfo;

    constexpr static uint32_t k_notUsingImGui = std::numeric_limits<uint32_t>::max();
    /**
     * @brief Index of the subpass within main RenderPass which ImGui will be
     * rendered in or k_notUsingImGui to denote that ImGui will not be used
     * within this room
     */
    uint32_t imGuiSubpassIndex = k_notUsingImGui;
};

} // namespace re
