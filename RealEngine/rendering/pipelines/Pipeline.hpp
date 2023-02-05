/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/renderer/VulkanObject.hpp>
#include <RealEngine/rendering/pipelines/PipelineSources.hpp>

namespace RE {

struct PipelineGraphicsCreateInfo {
    vk::PipelineLayout pipelineLayout = nullptr;
    vk::PipelineVertexInputStateCreateInfo vertexInput{};
    vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList;
    bool enablePrimitiveRestart = false;
    uint32_t patchControlPoints = 0;
    vk::SpecializationInfo specializationInfo{};
};

struct PipelineComputeCreateInfo {
    vk::PipelineLayout pipelineLayout = nullptr;
};

/**
 * @brief Controls how vertices are rendered to screen.
*/
class Pipeline : public VulkanObject {
    friend class DescriptorSet;
public:

    /**
     * @brief Constructs graphics pipeline
    */
    Pipeline(const PipelineGraphicsCreateInfo& createInfo, const PipelineGraphicsSources& srcs);

    /**
     * @brief Constructs compute pipeline
    */
    Pipeline(const PipelineComputeCreateInfo& createInfo, const PipelineComputeSources& srcs);

    Pipeline(const Pipeline&) = delete;                         /**< Noncopyable */
    Pipeline& operator=(const Pipeline&) = delete;              /**< Noncopyable */

    Pipeline(Pipeline&& other) noexcept;                        /**< Movable */
    Pipeline& operator=(Pipeline&& other) noexcept;             /**< Movable */

    ~Pipeline();

    const vk::Pipeline& operator*() { return m_pipeline; }
    const vk::Pipeline* operator->() { return &m_pipeline; }

    const vk::Pipeline& pipeline() const { return m_pipeline; }

private:

    vk::Pipeline m_pipeline{};
};

}
