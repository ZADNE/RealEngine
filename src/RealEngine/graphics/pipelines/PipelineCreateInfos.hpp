/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/graphics/output_control/RenderPassSubpass.hpp>
#include <RealEngine/utility/DebugString.hpp>

namespace re {

struct PipelineGraphicsCreateInfo {
    // Specialization
    const vk::SpecializationInfo* specializationInfo = nullptr;

    // Input assembly
    const vk::PipelineVertexInputStateCreateInfo* vertexInput = nullptr;
    vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList;
    bool enablePrimitiveRestart    = false;

    // Tessellation
    uint32_t patchControlPoints = 0; // Zero means that tessellation is not used

    // Rasterization
    vk::PolygonMode polygonMode = vk::PolygonMode::eFill;
    vk::CullModeFlags cullMode  = vk::CullModeFlagBits::eNone;
    float lineWidth             = 1.0f;

    bool enableDepth = false;
    bool enableBlend = true;

    vk::PipelineLayout pipelineLayout = nullptr;
    RenderPassSubpass renderPassSubpass{};

    // Debug
    [[no_unique_address]] DebugString<> debugName;
};

struct PipelineComputeCreateInfo {
    // Specialization
    const vk::SpecializationInfo* specializationInfo = nullptr;

    vk::PipelineLayout pipelineLayout = nullptr;

    // Debug
    [[no_unique_address]] DebugString<> debugName;
};

} // namespace re
