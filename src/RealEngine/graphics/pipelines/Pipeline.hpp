/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/graphics/pipelines/PipelineSources.hpp>
#include <RealEngine/renderer/VulkanObject.hpp>

namespace re {

struct PipelineGraphicsCreateInfo {
    // Specialization
    const vk::SpecializationInfo* specializationInfo = nullptr;

    // Input assembly
    const vk::PipelineVertexInputStateCreateInfo* vertexInput = nullptr;
    vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList;
    bool                  enablePrimitiveRestart = false;

    // Tessellation
    uint32_t patchControlPoints = 0; // Zero means that tesselation is not used

    // Rasterization
    vk::CullModeFlags cullMode  = vk::CullModeFlagBits::eNone;
    float             lineWidth = 1.0f;

    bool enableDepth = false;
    bool enableBlend = true;

    vk::PipelineLayout pipelineLayout = nullptr;
    vk::RenderPass renderPass = nullptr; // The main renderpass is used if unspecified
    uint32_t subpassIndex = 0;
};

struct PipelineComputeCreateInfo {
    // Specialization
    const vk::SpecializationInfo* specializationInfo = nullptr;
    vk::PipelineLayout            pipelineLayout     = nullptr;
};

/**
 * @brief Controls how vertices are rendered to screen.
 */
class Pipeline: public VulkanObject {
public:
    /**
     * @brief Constructs a null pipeline that cannot be used for rendering or compute
     */
    explicit Pipeline() {}

    /**
     * @brief Constructs graphics pipeline
     */
    Pipeline(
        const PipelineGraphicsCreateInfo& createInfo,
        const PipelineGraphicsSources&    srcs
    );

    /**
     * @brief Constructs compute pipeline
     */
    Pipeline(
        const PipelineComputeCreateInfo& createInfo, const PipelineComputeSources& srcs
    );

    Pipeline(const Pipeline&)            = delete; /**< Noncopyable */
    Pipeline& operator=(const Pipeline&) = delete; /**< Noncopyable */

    Pipeline(Pipeline&& other) noexcept;            /**< Movable */
    Pipeline& operator=(Pipeline&& other) noexcept; /**< Movable */

    ~Pipeline();

    const vk::Pipeline& operator*() const { return m_pipeline; }
    const vk::Pipeline* operator->() const { return &m_pipeline; }

    const vk::Pipeline& pipeline() const { return m_pipeline; }

private:
    vk::Pipeline m_pipeline{};
};

} // namespace re
