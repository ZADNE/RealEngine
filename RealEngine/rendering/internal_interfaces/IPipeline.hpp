/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vulkan/vulkan.hpp>

#include <RealEngine/rendering/internal_interfaces/IBuffer.hpp>
#include <RealEngine/rendering/internal_interfaces/IShaderProgram.hpp>

namespace RE {

class PipelineID {
    friend class VK13Pipeline;
public:

    ~PipelineID() = default;

    PipelineID(const PipelineID&) = delete;
    PipelineID(PipelineID&& other) noexcept :
        m_(other.m_) {
        std::memset(&other, 0, sizeof(other));
    }

    PipelineID& operator=(const PipelineID&) = delete;
    PipelineID& operator=(PipelineID&& other) noexcept {
        std::swap(m_, other.m_);
        return *this;
    }

private:

    struct VK13 {
        vk::DescriptorSetLayout descriptorSetLayout;
        vk::PipelineLayout pipelineLayout;
        vk::Pipeline pipeline;
    };

    PipelineID(const VK13& vk13) :
        m_({.vk13 = vk13}) {}

    union {
        VK13 vk13;
    } m_;
};

class IPipeline {
public:

    virtual PipelineID construct(const vk::PipelineVertexInputStateCreateInfo& vi, const ShaderProgramSources& srcs) const = 0;
    virtual void destruct(PipelineID& pl) const = 0;
};

}