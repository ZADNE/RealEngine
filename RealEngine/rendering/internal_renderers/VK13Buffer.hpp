/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IBuffer.hpp>

namespace RE {

/**
* @brief Is Vulkan 1.3 implementation of Buffer
*
* Do not use this directly - use Buffer class instead.
*/
class VK13Buffer final : public IBuffer {
    friend class VK13Fixture;
public:

    BufferID construct(size_t sizeInBytes, vk::BufferUsageFlags usage, const void* data) const override;
    void destruct(BufferID& bf) const override;

private:

    VK13Buffer(
        const vk::PhysicalDevice& physicalDevice, const vk::Device& device,
        const vk::Queue& graphicsQueue, const vk::CommandPool& commandPool) :
        m_physicalDevice(physicalDevice), m_device(device),
        m_graphicsQueue(graphicsQueue), m_commandPool(commandPool) {
    }

    uint32_t selectMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;
    BufferID createBufferAndMemory(size_t sizeInBytes, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) const;
    void copyBetweenBuffers(const vk::Buffer& src, const vk::Buffer& dst, const vk::BufferCopy& copyInfo) const;

    const vk::PhysicalDevice& m_physicalDevice;
    const vk::Device& m_device;
    const vk::Queue& m_graphicsQueue;
    const vk::CommandPool& m_commandPool;
};

}