/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vulkan/vulkan.hpp>

#include <RealEngine/rendering/internal_interfaces/IBuffer.hpp>
#include <RealEngine/rendering/internal_interfaces/IShaderProgram.hpp>

namespace RE {

class PipelineID {
    friend class VK13Pipeline; friend class GL46Pipeline;
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
    struct GL46 {
        unsigned int va;
        unsigned int sp;
    };

    PipelineID(const VK13& vk) :
        m_({.vk = vk}) {}
    PipelineID(const GL46& gl) :
        m_({.gl = gl}) {}

    union {
        VK13 vk;
        GL46 gl;
    } m_;
};

class IPipeline {
public:

    virtual PipelineID construct(const vk::PipelineVertexInputStateCreateInfo& vi, const ShaderProgramSources& srcs) const = 0;
    virtual void destruct(PipelineID& pl) const = 0;
};

}