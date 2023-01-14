/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/DescriptorSet.hpp>

namespace RE {

DescriptorSet::DescriptorSet(const Pipeline& pl):
    m_descriptorSet(s_device->allocateDescriptorSets(vk::DescriptorSetAllocateInfo{
        *s_descriptorPool, pl.m_descriptorSetLayout
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
    //s_device->freeDescriptorSets(*s_descriptorPool, m_descriptorSet);
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

void DescriptorSet::write(vk::DescriptorType type, uint32_t binding, uint32_t arrayIndex, const Texture& tex, vk::ImageLayout layout/* = vk::ImageLayout::eShaderReadOnlyOptimal*/) {
    auto imageInfo = vk::DescriptorImageInfo{
        tex.m_sampler,
        tex.m_imageView,
        layout
    };
    s_device->updateDescriptorSets(
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
