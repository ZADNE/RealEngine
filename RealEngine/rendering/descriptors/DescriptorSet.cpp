/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/descriptors/DescriptorSet.hpp>

namespace RE {

DescriptorSet::DescriptorSet(const PipelineLayout& pipelineLayout, uint32_t setIndex):
    m_descriptorSet(device().allocateDescriptorSets(vk::DescriptorSetAllocateInfo{
        descriptorPool(), pipelineLayout.descriptorSetLayout(setIndex)
    }).back()) {
}

DescriptorSet::DescriptorSet(DescriptorSet&& other) noexcept:
    m_descriptorSet(other.m_descriptorSet) {
    other.m_descriptorSet = nullptr;
}

DescriptorSet& DescriptorSet::operator=(DescriptorSet&& other) noexcept {
    std::swap(m_descriptorSet, other.m_descriptorSet);
    return *this;
}

DescriptorSet::~DescriptorSet() {
    //Do not free the descriptor, it is 'freed' when the pool it was created from is destroyed
    //device().freeDescriptorSets(descriptorPool(), m_descriptorSet);
}

void DescriptorSet::write(vk::DescriptorType type, uint32_t binding, const Buffer& bf, vk::DeviceSize offset, vk::DeviceSize range) {
    auto bufferInfo = vk::DescriptorBufferInfo{
        bf.buffer(),
        offset,
        range
    };
    device().updateDescriptorSets(
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

void DescriptorSet::write(vk::DescriptorType type, uint32_t binding, uint32_t arrayIndex, const Texture& tex, vk::ImageLayout layout/* = vk::ImageLayout::eShaderReadOnlyOptimal*/) {
    auto imageInfo = vk::DescriptorImageInfo{
        tex.sampler(),
        tex.imageView(),
        layout
    };
    device().updateDescriptorSets(
        vk::WriteDescriptorSet{
            m_descriptorSet,
            binding,
            arrayIndex,
            type,
            imageInfo,
            {},
            {}
        },
        {}
    );
}

}
