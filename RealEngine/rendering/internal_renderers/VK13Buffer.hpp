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
public:
    //Fixture functions
    VK13Buffer(
        const vk::PhysicalDevice& physicalDevice, const vk::Device& device,
        const vk::Queue& graphicsQueue, const vk::CommandPool& commandPool);
    void setCommandBuffer(const vk::CommandBuffer* commandBuffer);

    //IBuffer implementation
    BufferID construct(size_t sizeInBytes, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memProperty, const void* data) const override;
    void destruct(BufferID& bf) const override;

    void* map(const BufferID& bf, size_t offsetInBytes, size_t lengthInBytes) const override;
    void unmap(const BufferID& bf) const override;

    void bindAsVertexBuffer(const BufferID& bf, uint32_t binding, uint64_t offsetInBytes) const override;
    void bindAsIndexBuffer(const BufferID& bf, uint64_t offsetInBytes, vk::IndexType indexType) const override;

private:

    uint32_t selectMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;
    struct BufferAndMemory {
        vk::Buffer buffer = nullptr;
        vk::DeviceMemory memory = nullptr;
    };
    BufferAndMemory createBufferAndMemory(size_t sizeInBytes, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) const;
    void copyBetweenBuffers(const vk::Buffer& src, const vk::Buffer& dst, const vk::BufferCopy& copyInfo) const;

    const vk::PhysicalDevice& m_physicalDevice;
    const vk::Device& m_device;
    const vk::Queue& m_graphicsQueue;
    const vk::CommandPool& m_commandPool;
    const vk::CommandBuffer* m_commandBuffer = nullptr;
};

}