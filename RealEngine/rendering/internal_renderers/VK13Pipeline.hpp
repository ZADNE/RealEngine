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
    friend class VK13Fixture;
public:

    PipelineID construct(const vk::PipelineVertexInputStateCreateInfo& vi, const ShaderProgramSources& srcs) const override;
    void destruct(PipelineID& pl) const override;

private:

    VK13Pipeline(const vk::Device& device, const vk::PipelineCache& pipelineCache, const vk::RenderPass& renderPass) :
        m_device(device), m_pipelineCache(pipelineCache), m_renderPass(renderPass) {
    }

    void reflect(const ShaderSourceRef& src, vk::ShaderStageFlagBits st, std::vector<vk::DescriptorSetLayoutBinding>& dslbs) const;

    const vk::Device& m_device;
    const vk::PipelineCache& m_pipelineCache;
    const vk::RenderPass& m_renderPass;
};

}