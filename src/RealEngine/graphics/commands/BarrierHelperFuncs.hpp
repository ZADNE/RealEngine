/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vulkan/vulkan.hpp>

namespace re {

/**
 * @brief Is a helper function that fills rarely chaning fields
 */
constexpr vk::ImageMemoryBarrier2 imageMemoryBarrier(
    vk::PipelineStageFlags2 srcStageMask, vk::AccessFlags2 srcAccessMask,
    vk::PipelineStageFlags2 dstStageMask, vk::AccessFlags2 dstAccessMask,
    vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::Image image,
    vk::ImageSubresourceRange range =
        vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
) {
    return vk::ImageMemoryBarrier2{
        srcStageMask,
        srcAccessMask,
        dstStageMask,
        dstAccessMask,
        oldLayout,
        newLayout,
        vk::QueueFamilyIgnored,
        vk::QueueFamilyIgnored, // No ownership transition
        image,
        range
    };
}

/**
 * @brief Is a helper function that fills rarely chaning fields
 */
constexpr vk::BufferMemoryBarrier2 bufferMemoryBarrier(
    vk::PipelineStageFlags2 srcStageMask, vk::AccessFlags2 srcAccessMask,
    vk::PipelineStageFlags2 dstStageMask, vk::AccessFlags2 dstAccessMask,
    vk::Buffer buffer, vk::DeviceSize offset = 0, vk::DeviceSize size = vk::WholeSize
) {
    return vk::BufferMemoryBarrier2{
        srcStageMask,
        srcAccessMask,
        dstStageMask,
        dstAccessMask,
        vk::QueueFamilyIgnored,
        vk::QueueFamilyIgnored, // No ownership transition
        buffer,
        offset,
        size
    };
}

/**
 * @brief Is a helper function that creates a debug all commands to all commands
 * global memory barrier
 */
constexpr vk::MemoryBarrier2 debugBarrier() {
    return vk::MemoryBarrier2{
        vk::PipelineStageFlagBits2::eAllCommands,
        vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite,
        vk::PipelineStageFlagBits2::eAllCommands,
        vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite,
    };
}

} // namespace re
