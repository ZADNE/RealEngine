/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IDescriptorSet.hpp>

namespace RE {

/**
* @brief Is Vulkan 1.3 implementation of DescriptorSet
*
* Do not use this directly - use DescriptorSet class instead.
*/
class VK13DescriptorSet final : public IDescriptorSet {
public:
    //Fixture functions
    VK13DescriptorSet(
        const vk::Device& device, const vk::DescriptorPool& descriptorPool);

    //Interface implementation
    DescriptorSetID construct(const PipelineID& pl, vk::DescriptorType type, uint32_t binding, const BufferID& bf, vk::DeviceSize offset, vk::DeviceSize range) const override;
    void destruct(DescriptorSetID& ds) const override;

private:

    const vk::Device& m_device;
    const vk::DescriptorPool& m_descriptorPool;

};

}