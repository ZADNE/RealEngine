/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vulkan/vulkan.hpp>

#include <RealEngine/rendering/internal_interfaces/IShaderProgram.hpp>

namespace RE {

class PipelineID {
    friend class VK13Pipeline;
    friend class VK13DescriptorSet;
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
        vk::DescriptorSetLayout descriptorSetLayout = nullptr;
        vk::PipelineLayout pipelineLayout = nullptr;
        vk::Pipeline pipeline = nullptr;
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

    virtual void bind(const PipelineID& pl, vk::PipelineBindPoint bindPoint) const = 0;
    virtual void draw(const PipelineID& pl, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const = 0;
};

}