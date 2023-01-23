/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/pipelines/PipelineSources.hpp>

namespace RE {

struct PipelineCreateInfo {
    vk::PipelineVertexInputStateCreateInfo vertexInput{};
    vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList;
    bool enablePrimitiveRestart = false;
    uint32_t patchControlPoints = 0;
    std::vector<vk::DescriptorBindingFlags> descriptorBindingFlags{};
    vk::SpecializationInfo specializationInfo{};
};

/**
 * @brief Controls how vertices are rendered to screen.
*/
class Pipeline {
    friend class VulkanFixture;
    friend class DescriptorSet;
public:

    /**
     * @brief Constructs graphics pipeline
    */
    Pipeline(const PipelineCreateInfo& createInfo, const PipelineSources& srcs);

    /**
     * @brief Constructs compute pipeline
    */
    Pipeline(const ShaderSourceRef& compute);

    Pipeline(const Pipeline&) = delete;
    Pipeline(Pipeline&& other) noexcept;

    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline& operator=(Pipeline&& other) noexcept;

    ~Pipeline();

    const vk::Pipeline& operator*() { return m_pipeline; }
    const vk::Pipeline* operator->() { return &m_pipeline; }

    const vk::Pipeline& pipeline() const { return m_pipeline; }
    const vk::PipelineLayout& pipelineLayout() const { return m_pipelineLayout; }

private:

    void reflect(
        const ShaderSourceRef& src,
        vk::ShaderStageFlagBits st,
        const vk::SpecializationInfo& specInfo,
        std::vector<vk::DescriptorSetLayoutBinding>& dslbs,
        std::vector<vk::PushConstantRange>& ranges
    ) const;

    vk::DescriptorSetLayout m_descriptorSetLayout{};
    vk::PipelineLayout m_pipelineLayout{};
    vk::Pipeline m_pipeline{};

    static inline const vk::Device* s_device = nullptr;
    static inline const vk::PipelineCache* s_pipelineCache = nullptr;
    static inline const vk::RenderPass* s_renderPass = nullptr;
    static inline const vk::CommandBuffer* s_commandBuffer = nullptr;
};

}
