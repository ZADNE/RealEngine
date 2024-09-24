/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/graphics/buffers/Buffer.hpp>
#include <RealEngine/graphics/textures/Texture.hpp>
#include <RealEngine/renderer/VulkanObjectBase.hpp>

namespace re {

/**
 * @brief Specifies parameters for DescriptorSet creation
 */
struct DescriptorSetCreateInfo {
    vk::DescriptorSetLayout layout{};

    // Debug
    [[no_unique_address]] DebugName<> debugName;
};

/**
 * @brief Refers to concrete resources to be used by a pipeline
 */
class DescriptorSet: public VulkanObjectBase {
public:
    /**
     * @brief Constructs a null descriptor set that does not describe anything
     */
    explicit DescriptorSet() {}

    /**
     * @brief Constructs set for given layout
     */
    explicit DescriptorSet(const DescriptorSetCreateInfo& createInfo);

    DescriptorSet(const DescriptorSet&)            = delete;  ///< Noncopyable
    DescriptorSet& operator=(const DescriptorSet&) = delete;  ///< Noncopyable

    DescriptorSet(DescriptorSet&& other) noexcept;            ///< Movable
    DescriptorSet& operator=(DescriptorSet&& other) noexcept; ///< Movable

    ~DescriptorSet();

    void write(
        vk::DescriptorType type, uint32_t binding, uint32_t arrayIndex,
        const Buffer& buf, vk::DeviceSize offset = 0ull,
        vk::DeviceSize range = vk::WholeSize
    );
    void write(
        vk::DescriptorType type, uint32_t binding, uint32_t arrayIndex,
        const Texture& tex, vk::ImageLayout layout
    );
    void write(
        vk::DescriptorType type, uint32_t binding, uint32_t arrayIndex,
        const vk::DescriptorImageInfo& imageInfo
    );

    const vk::DescriptorSet& operator*() const { return m_descriptorSet; }
    const vk::DescriptorSet* operator->() const { return &m_descriptorSet; }

    const vk::DescriptorSet& descriptorSet() const { return m_descriptorSet; }

private:
    vk::DescriptorSet m_descriptorSet{};
};

} // namespace re
