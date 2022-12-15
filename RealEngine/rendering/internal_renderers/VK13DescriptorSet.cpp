/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/VK13DescriptorSet.hpp>

namespace RE {

VK13DescriptorSet::VK13DescriptorSet(
    const vk::Device& device, const vk::DescriptorPool& descriptorPool) :
    m_device(device), m_descriptorPool(descriptorPool) {
}

void VK13DescriptorSet::setCommandBuffer(const vk::CommandBuffer* commandBuffer) {
    m_commandBuffer = commandBuffer;
}

DescriptorSetID VK13DescriptorSet::construct(const PipelineID& pl) const {
    auto ds = m_device.allocateDescriptorSets(vk::DescriptorSetAllocateInfo{
        m_descriptorPool, pl.m_.vk13.descriptorSetLayout
        }).back();
    return DescriptorSetID::VK13{.descriptorSet = ds};
}

void VK13DescriptorSet::destruct(DescriptorSetID& ds) const {
    //No operation is required, destriptor sets are freed when their pool is freed
    (void)ds;
}

void VK13DescriptorSet::write(const DescriptorSetID& ds, vk::DescriptorType type, uint32_t binding, const BufferID& bf, vk::DeviceSize offset, vk::DeviceSize range) const {
    auto bufferInfo = vk::DescriptorBufferInfo{
        bf.m_.vk13.mainBuffer,
        offset,
        range
    };
    m_device.updateDescriptorSets(
        vk::WriteDescriptorSet{
            ds.m_.vk13.descriptorSet,
            binding,
            0u,
            type,
            {},
            bufferInfo,
            {}
        },
        {}
    );
}

void VK13DescriptorSet::bind(const DescriptorSetID& ds, vk::PipelineBindPoint bindPoint, const PipelineID& pl) const {
    m_commandBuffer->bindDescriptorSets(bindPoint, pl.m_.vk13.pipelineLayout, 0u, ds.m_.vk13.descriptorSet, {});
}

}