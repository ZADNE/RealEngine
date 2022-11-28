/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IPipeline.hpp>

namespace RE {

/**
* @brief Is Vulkan 1.3 implementation of Pipeline class.
*
* Do not use this directly - use Pipeline instead.
*/
class VK13Pipeline final : public IPipeline {
public:
    //Fixture functions
    VK13Pipeline(
        const vk::Device& device, const vk::PipelineCache& pipelineCache,
        const vk::RenderPass& renderPass);
    void setCommandBuffer(const vk::CommandBuffer* commandBuffer);

    //IPipeline implementation
    PipelineID construct(const vk::PipelineVertexInputStateCreateInfo& vi, const ShaderProgramSources& srcs) const override;
    void destruct(PipelineID& pl) const override;

    void bind(const PipelineID& pl, vk::PipelineBindPoint bindPoint) const override;
    void draw(const PipelineID& pl, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const override;

private:

    void reflect(const ShaderSourceRef& src, vk::ShaderStageFlagBits st, std::vector<vk::DescriptorSetLayoutBinding>& dslbs) const;

    const vk::Device& m_device;
    const vk::PipelineCache& m_pipelineCache;
    const vk::RenderPass& m_renderPass;
    const vk::CommandBuffer* m_commandBuffer = nullptr;
};

}