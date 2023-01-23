/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vulkan/vulkan.hpp>

#include <RealEngine/rendering/pipelines/Pipeline.hpp>
#include <RealEngine/rendering/buffers/Buffer.hpp>
#include <RealEngine/rendering/textures/Texture.hpp>

namespace RE {

class DescriptorSet {
    friend class VulkanFixture;
public:

    DescriptorSet(const Pipeline& pl);

    DescriptorSet(const DescriptorSet&) = delete;
    DescriptorSet(DescriptorSet&& other) noexcept;

    DescriptorSet& operator=(const DescriptorSet&) = delete;
    DescriptorSet& operator=(DescriptorSet&& other) noexcept;

    ~DescriptorSet();

    void write(vk::DescriptorType type, uint32_t binding, const Buffer& bf, vk::DeviceSize offset, vk::DeviceSize range);
    void write(vk::DescriptorType type, uint32_t binding, uint32_t arrayIndex, const Texture& tex, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal);

    const vk::DescriptorSet& operator*() const { return m_descriptorSet; }
    const vk::DescriptorSet* operator->() const { return &m_descriptorSet; }

    const vk::DescriptorSet& descriptorSet() const { return m_descriptorSet; }

private:

    vk::DescriptorSet m_descriptorSet{};

    static inline const vk::Device* s_device = nullptr;
    static inline const vk::DescriptorPool* s_descriptorPool = nullptr;
    static inline const vk::CommandBuffer* s_commandBuffer = nullptr;
};

}
