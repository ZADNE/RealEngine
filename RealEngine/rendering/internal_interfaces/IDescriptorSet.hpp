/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vulkan/vulkan.hpp>

#include <RealEngine/rendering/internal_interfaces/IPipeline.hpp>
#include <RealEngine/rendering/internal_interfaces/IBuffer.hpp>

namespace RE {

/**
 * @brief Contains all members of DescriptorSet
 * @note For internal usage in RealEngine.
*/
class DescriptorSetID {
    friend class VK13DescriptorSet;
public:

    ~DescriptorSetID() = default;

    DescriptorSetID(const DescriptorSetID&) = delete;
    DescriptorSetID(DescriptorSetID&& other) noexcept :
        m_(other.m_) {
        std::memset(&other, 0, sizeof(other));
    }

    DescriptorSetID& operator=(const DescriptorSetID&) = delete;
    DescriptorSetID& operator=(DescriptorSetID&& other) noexcept {
        std::swap(m_, other.m_);
        return *this;
    }

private:

    struct VK13 {
        vk::DescriptorSet descriptorSet = nullptr;
    };

    DescriptorSetID(const VK13& vk13) :
        m_({.vk13 = vk13}) {}

    union {
        VK13 vk13;
    } m_;
};

/**
* @brief Is a renderer-agnostic interface to DescriptorSet's implementation.
*
* This is used internally by DescriptorSet.
*
* @see RE::DescriptorSet
*/
class IDescriptorSet {
public:

    virtual DescriptorSetID construct(const PipelineID& pl) const = 0;
    virtual void destruct(DescriptorSetID& ds) const = 0;

    virtual void write(const DescriptorSetID& ds, vk::DescriptorType type, uint32_t binding, const BufferID& bf, vk::DeviceSize offset, vk::DeviceSize range) const = 0;

    virtual void bind(const DescriptorSetID& ds, vk::PipelineBindPoint bindPoint, const PipelineID& pl) const = 0;
};

}