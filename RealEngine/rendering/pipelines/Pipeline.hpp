/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/pipelines/PipelineSources.hpp>

namespace RE {

/**
 * @brief Controls how vertices are rendered to screen.
*/
class Pipeline {
    friend class VK13Fixture;
    friend class DescriptorSet;
public:

    /**
     * @brief Constructs graphics pipeline
    */
    Pipeline(const vk::PipelineVertexInputStateCreateInfo& vi, const vk::PipelineInputAssemblyStateCreateInfo& ia, const PipelineSources& srcs, uint32_t patchControlPoints = 0);

    /**
     * @brief Constructs compute pipeline
    */
    Pipeline(const ShaderSourceRef& compute);

    Pipeline(const Pipeline&) = delete;
    Pipeline(Pipeline&& other) noexcept;

    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline& operator=(Pipeline&& other) noexcept;

    ~Pipeline();

    void bind(vk::PipelineBindPoint bindPoint) const;

    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const;
    void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) const;

    const vk::Pipeline& pipeline() const { return m_pipeline; }
    const vk::PipelineLayout& pipelineLayout() const { return m_pipelineLayout; }

private:

    void reflect(const ShaderSourceRef& src, vk::ShaderStageFlagBits st, std::vector<vk::DescriptorSetLayoutBinding>& dslbs, std::vector<vk::PushConstantRange>& ranges) const;

    vk::DescriptorSetLayout m_descriptorSetLayout{};
    vk::PipelineLayout m_pipelineLayout{};
    vk::Pipeline m_pipeline{};

    static inline const vk::Device* s_device = nullptr;
    static inline const vk::PipelineCache* s_pipelineCache = nullptr;
    static inline const vk::RenderPass* s_renderPass = nullptr;
    static inline const vk::CommandBuffer* s_commandBuffer = nullptr;
};

}
