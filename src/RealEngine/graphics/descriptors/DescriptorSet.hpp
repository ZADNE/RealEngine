/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/graphics/buffers/Buffer.hpp>
#include <RealEngine/graphics/textures/Texture.hpp>
#include <RealEngine/renderer/VulkanObject.hpp>

namespace re {

/**
 * @brief Describes (= refers to) concrete resources (= buffers, images, etc)
 */
class DescriptorSet: public VulkanObject {
public:
    /**
     * @brief Constructs set for given layout
     */
    explicit DescriptorSet(const vk::DescriptorSetLayout& descSetLayout);

    DescriptorSet(const DescriptorSet&)            = delete; /**< Noncopyable */
    DescriptorSet& operator=(const DescriptorSet&) = delete; /**< Noncopyable */

    DescriptorSet(DescriptorSet&& other) noexcept;            /**< Movable */
    DescriptorSet& operator=(DescriptorSet&& other) noexcept; /**< Movable */

    ~DescriptorSet();

    void write(
        vk::DescriptorType type,
        uint32_t           binding,
        uint32_t           arrayIndex,
        const Buffer&      buf,
        vk::DeviceSize     offset,
        vk::DeviceSize     range
    );
    void write(
        vk::DescriptorType type,
        uint32_t           binding,
        uint32_t           arrayIndex,
        const Texture&     tex,
        vk::ImageLayout    layout
    );

    const vk::DescriptorSet& operator*() const { return m_descriptorSet; }
    const vk::DescriptorSet* operator->() const { return &m_descriptorSet; }

    const vk::DescriptorSet& descriptorSet() const { return m_descriptorSet; }

private:
    vk::DescriptorSet m_descriptorSet{};
};

} // namespace re
