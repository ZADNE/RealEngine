/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/VK13DescriptorSet.hpp>

namespace RE {

VK13DescriptorSet::VK13DescriptorSet(
    const vk::Device& device, const vk::DescriptorPool& descriptorPool) :
    m_device(device), m_descriptorPool(descriptorPool) {
}

DescriptorSetID VK13DescriptorSet::construct(const PipelineID& pl, vk::DescriptorType type, uint32_t binding, const BufferID& bf, vk::DeviceSize offset, vk::DeviceSize range) const {
    //Allocate the set
    auto ds = m_device.allocateDescriptorSets(vk::DescriptorSetAllocateInfo{
        m_descriptorPool, pl.m_.vk13.descriptorSetLayout
    }).back();
    //Update the set
    auto bufferInfo = vk::DescriptorBufferInfo{
        bf.m_.vk13.mainBuffer,
        offset,
        range
    };
    m_device.updateDescriptorSets(vk::WriteDescriptorSet{
        ds,
        binding,
        0u,
        type,
        {},
        bufferInfo,
        {}
    }, {});
    return DescriptorSetID::VK13{.desciptorSet = ds};
}

void VK13DescriptorSet::destruct(DescriptorSetID& ds) const {
    //No operation is required, destriptor sets are freed when their pool is freed
    (void)ds;
}

}