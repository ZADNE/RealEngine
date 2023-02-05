/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/renderer/VulkanObject.hpp>
#include <RealEngine/rendering/pipelines/PipelineLayout.hpp>
#include <RealEngine/rendering/buffers/Buffer.hpp>
#include <RealEngine/rendering/textures/Texture.hpp>

namespace RE {

/**
 * @brief Describes (= refers to) concrete resources (= buffers, images, etc)
*/
class DescriptorSet: public VulkanObject {
public:

    /**
     * @brief Constructs set for given layout
     * @param pipelineLayout Array of set layouts to select from
     * @param setIndex Index of the set leyout within the pipeline layout
    */
    DescriptorSet(const PipelineLayout& pipelineLayout, uint32_t setIndex);

    DescriptorSet(const DescriptorSet&) = delete;               /**< Noncopyable */
    DescriptorSet& operator=(const DescriptorSet&) = delete;    /**< Noncopyable */

    DescriptorSet(DescriptorSet&& other) noexcept;              /**< Movable */
    DescriptorSet& operator=(DescriptorSet&& other) noexcept;   /**< Movable */

    ~DescriptorSet();

    void write(vk::DescriptorType type, uint32_t binding, const Buffer& bf, vk::DeviceSize offset, vk::DeviceSize range);
    void write(vk::DescriptorType type, uint32_t binding, uint32_t arrayIndex, const Texture& tex, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal);

    const vk::DescriptorSet& operator*() const { return m_descriptorSet; }
    const vk::DescriptorSet* operator->() const { return &m_descriptorSet; }

    const vk::DescriptorSet& descriptorSet() const { return m_descriptorSet; }

private:

    vk::DescriptorSet m_descriptorSet{};
};

}
