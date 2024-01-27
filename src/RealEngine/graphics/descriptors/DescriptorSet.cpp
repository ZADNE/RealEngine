/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/descriptors/DescriptorSet.hpp>

namespace re {

DescriptorSet::DescriptorSet(const DescriptorSetCreateInfo& createInfo)
    : m_descriptorSet(device()
                          .allocateDescriptorSets(vk::DescriptorSetAllocateInfo{
                              descriptorPool(), createInfo.layout})
                          .back()) {

    setDebugUtilsObjectName(m_descriptorSet, createInfo.debugName);
}

DescriptorSet::DescriptorSet(DescriptorSet&& other) noexcept
    : m_descriptorSet(other.m_descriptorSet) {
    other.m_descriptorSet = nullptr;
}

DescriptorSet& DescriptorSet::operator=(DescriptorSet&& other) noexcept {
    std::swap(m_descriptorSet, other.m_descriptorSet);
    return *this;
}

DescriptorSet::~DescriptorSet() {
    // Do not free the descriptor
    // It is 'freed' when the pool it was created from is destroyed
}

void DescriptorSet::write(
    vk::DescriptorType type,
    uint32_t           binding,
    uint32_t           arrayIndex,
    const Buffer&      buf,
    vk::DeviceSize     offset,
    vk::DeviceSize     range
) {
    auto bufferInfo = vk::DescriptorBufferInfo{buf.buffer(), offset, range};
    device().updateDescriptorSets(
        vk::WriteDescriptorSet{
            m_descriptorSet, binding, arrayIndex, type, {}, bufferInfo, {}},
        {}
    );
}

void DescriptorSet::write(
    vk::DescriptorType type,
    uint32_t           binding,
    uint32_t           arrayIndex,
    const Texture&     tex,
    vk::ImageLayout    layout
) {
    auto imageInfo = vk::DescriptorImageInfo{tex.sampler(), tex.imageView(), layout};
    device().updateDescriptorSets(
        vk::WriteDescriptorSet{
            m_descriptorSet, binding, arrayIndex, type, imageInfo, {}, {}},
        {}
    );
}

void DescriptorSet::write(
    vk::DescriptorType             type,
    uint32_t                       binding,
    uint32_t                       arrayIndex,
    const vk::DescriptorImageInfo& imageInfo
) {
    device().updateDescriptorSets(
        vk::WriteDescriptorSet{
            m_descriptorSet, binding, arrayIndex, type, imageInfo, {}, {}},
        {}
    );
}

} // namespace re
