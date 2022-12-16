/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/DescriptorSet.hpp>

namespace RE {

DescriptorSet::DescriptorSet(const Pipeline& pl) :
    m_descriptorSet(s_device->allocateDescriptorSets(vk::DescriptorSetAllocateInfo{
        *s_descriptorPool, pl.m_descriptorSetLayout
        }).back()) {
}

DescriptorSet::DescriptorSet(DescriptorSet&& other) noexcept :
    m_descriptorSet(other.m_descriptorSet) {
    other.m_descriptorSet = nullptr;
}

DescriptorSet& DescriptorSet::operator=(DescriptorSet&& other) noexcept {
    std::swap(m_descriptorSet, other.m_descriptorSet);
    return *this;
}

DescriptorSet::~DescriptorSet() {
    //No operation is required, destriptor sets are freed when their pool is freed
}

void DescriptorSet::write(vk::DescriptorType type, uint32_t binding, const Buffer& bf, vk::DeviceSize offset, vk::DeviceSize range) {
    auto bufferInfo = vk::DescriptorBufferInfo{
        bf.m_buffer,
        offset,
        range
    };
    s_device->updateDescriptorSets(
        vk::WriteDescriptorSet{
            m_descriptorSet,
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

void DescriptorSet::bind(vk::PipelineBindPoint bindPoint, const Pipeline& pl) const {
    s_commandBuffer->bindDescriptorSets(bindPoint, pl.m_pipelineLayout, 0u, m_descriptorSet, {});
}

}
