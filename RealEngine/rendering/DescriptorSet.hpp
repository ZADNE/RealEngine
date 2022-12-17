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
    friend class VK13Fixture;
public:

    DescriptorSet(const Pipeline& pl);

    DescriptorSet(const DescriptorSet&) = delete;
    DescriptorSet(DescriptorSet&& other) noexcept;

    DescriptorSet& operator=(const DescriptorSet&) = delete;
    DescriptorSet& operator=(DescriptorSet&& other) noexcept;

    ~DescriptorSet();

    void write(vk::DescriptorType type, uint32_t binding, const Buffer& bf, vk::DeviceSize offset, vk::DeviceSize range);
    void write(vk::DescriptorType type, uint32_t binding, const Texture& tx);

    void bind(vk::PipelineBindPoint bindPoint, const Pipeline& pl) const;

private:

    vk::DescriptorSet m_descriptorSet{};

    static inline const vk::Device* s_device = nullptr;
    static inline const vk::DescriptorPool* s_descriptorPool = nullptr;
    static inline const vk::CommandBuffer* s_commandBuffer = nullptr;
};

}
