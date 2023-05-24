﻿/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/renderer/VulkanObject.hpp>
#include <RealEngine/rendering/pipelines/PipelineSources.hpp>

namespace RE {

struct PipelineLayoutCreateInfo {
    vk::ArrayProxy<vk::ArrayProxy<vk::DescriptorBindingFlags>> descriptorBindingFlags{};
    vk::SpecializationInfo specializationInfo{};
};

struct PipelineLayoutDescription {
    std::vector<std::vector<vk::DescriptorSetLayoutBinding>> bindings;
    std::vector<vk::PushConstantRange> ranges;
};

/**
 * @brief Creates interface between pipeline and other resources (textures, buffers, etc)
*/
class PipelineLayout: public VulkanObject {
public:

    /**
     * @brief Constructs pipeline layout by reflecting graphics pipeline sources
    */
    PipelineLayout(const PipelineLayoutCreateInfo& createInfo, const PipelineGraphicsSources& srcs);

    /**
     * @brief Constructs pipeline layout by reflecting compute pipeline sources
    */
    PipelineLayout(const PipelineLayoutCreateInfo& createInfo, const PipelineComputeSources& srcs);

    /**
     * @brief Constructs pipeline layout from given description
    */
    PipelineLayout(const PipelineLayoutCreateInfo& createInfo, const PipelineLayoutDescription& description);

    PipelineLayout(const PipelineLayout&) = delete;             /**< Noncopyable */
    PipelineLayout& operator=(const PipelineLayout&) = delete;  /**< Noncopyable */

    PipelineLayout(PipelineLayout&& other) noexcept;            /**< Movable */
    PipelineLayout& operator=(PipelineLayout&& other) noexcept; /**< Movable */

    ~PipelineLayout();

    const vk::PipelineLayout& operator*() const { return m_pipelineLayout; }
    const vk::PipelineLayout* operator->() const { return &m_pipelineLayout; }

    const vk::DescriptorSetLayout& descriptorSetLayout(uint32_t setIndex) const { return m_descriptorSetLayouts[setIndex]; }
    const vk::PipelineLayout& pipelineLayout() const { return m_pipelineLayout; }

private:

    /**
     * @brief Reflects all shaders
    */
    template<typename PipelineSources>
    PipelineLayoutDescription reflectSources(
        const PipelineSources& srcs,
        const vk::SpecializationInfo& specInfo
    ) const {
        PipelineLayoutDescription reflection;
        for (size_t st = 0; st < PipelineSources::k_numStages; ++st) {
            if (!srcs[st].vk13.empty()) {
                reflectSource(srcs[st], PipelineSources::stageFlags(st), specInfo, reflection);
            }
        }
        return reflection;
    }

    /**
     * @brief Reflects a single shader
    */
    void reflectSource(
        const ShaderSourceRef& src,
        vk::ShaderStageFlagBits st,
        const vk::SpecializationInfo& specInfo,
        PipelineLayoutDescription& description
    ) const;

    std::vector<vk::DescriptorSetLayout> m_descriptorSetLayouts{};
    vk::PipelineLayout m_pipelineLayout{};
};

}
