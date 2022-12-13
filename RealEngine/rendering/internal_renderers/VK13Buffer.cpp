/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/VK13Buffer.hpp>

#include <RealEngine/utility/Error.hpp>

namespace RE {

using enum vk::MemoryPropertyFlagBits;
using enum vk::BufferUsageFlagBits;

constexpr auto HOST_MEM = eHostVisible | eHostCoherent;

VK13Buffer::VK13Buffer(
    const vk::PhysicalDevice& physicalDevice, const vk::Device& device,
    const vk::Queue& graphicsQueue, const vk::CommandPool& commandPool) :
    m_physicalDevice(physicalDevice), m_device(device),
    m_graphicsQueue(graphicsQueue), m_commandPool(commandPool) {
}

void VK13Buffer::setCommandBuffer(const vk::CommandBuffer* commandBuffer) {
    m_commandBuffer = commandBuffer;
}

BufferID VK13Buffer::construct(size_t sizeInBytes, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memProperty, const void* data) const {
    BufferAndMemory main{};
    if (data && (memProperty & HOST_MEM) != HOST_MEM) {//If initial data are provided or stage is requested
        auto stage = createBufferAndMemory(sizeInBytes, eTransferSrc, HOST_MEM);
        main = createBufferAndMemory(sizeInBytes, usage | eTransferDst, memProperty);
        //Copy from data to staging buffer
        std::memcpy(m_device.mapMemory(stage.memory, 0, sizeInBytes), data, sizeInBytes);
        m_device.unmapMemory(stage.memory);
        //Copy from staging to final buffer
        copyBetweenBuffers(stage.buffer, main.buffer, vk::BufferCopy{0u, 0u, sizeInBytes});
        //Destruct the temporary stage
        m_device.destroyBuffer(stage.buffer);
        m_device.free(stage.memory);
    } else {//If no initial data are provided
        main = createBufferAndMemory(sizeInBytes, usage, memProperty);
    }
    return BufferID::VK13{
        .mainBuffer = main.buffer,
        .mainMemory = main.memory
    };
}

void VK13Buffer::destruct(BufferID& bf) const {
    m_device.destroyBuffer(bf.m_.vk13.mainBuffer);
    m_device.free(bf.m_.vk13.mainMemory);
}

void* VK13Buffer::map(const BufferID& bf, size_t offsetInBytes, size_t lengthInBytes) const {
    return m_device.mapMemory(bf.m_.vk13.mainMemory, offsetInBytes, lengthInBytes);
}

void VK13Buffer::unmap(const BufferID& bf) const {
    m_device.unmapMemory(bf.m_.vk13.mainMemory);
}

void VK13Buffer::bindAsVertexBuffer(const BufferID& bf, uint32_t binding, uint64_t offsetInBytes) const {
    m_commandBuffer->bindVertexBuffers(binding, bf.m_.vk13.mainBuffer, offsetInBytes);
}

uint32_t VK13Buffer::selectMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const {
    auto memProperties = m_physicalDevice.getMemoryProperties2().memoryProperties;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw Exception{"Could not find memory that suits the buffer!"};
}

VK13Buffer::BufferAndMemory VK13Buffer::createBufferAndMemory(size_t sizeInBytes, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) const {
    vk::BufferCreateInfo createInfo{{},
        sizeInBytes,
        usage,
        vk::SharingMode::eExclusive
    };
    auto buffer = m_device.createBuffer(createInfo);
    auto memReq = m_device.getBufferMemoryRequirements2({buffer}).memoryRequirements;
    auto memory = m_device.allocateMemory({
        std::max(sizeInBytes, memReq.alignment),
        selectMemoryType(memReq.memoryTypeBits, properties)
    });
    m_device.bindBufferMemory2(vk::BindBufferMemoryInfo{buffer, memory, 0u});
    return BufferAndMemory{.buffer = buffer, .memory = memory};
}

void VK13Buffer::copyBetweenBuffers(const vk::Buffer& src, const vk::Buffer& dst, const vk::BufferCopy& copyInfo) const {
    //Allocate temporary command buffer
    auto commandBuffer = m_device.allocateCommandBuffers({
        m_commandPool, vk::CommandBufferLevel::ePrimary, 1u
        }).front();
    //Record the copy
    commandBuffer.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    commandBuffer.copyBuffer(src, dst, copyInfo);
    commandBuffer.end();
    //Submit the copy
    m_graphicsQueue.submit(vk::SubmitInfo{nullptr, {}, commandBuffer});
    //Free the temporary buffer
    m_graphicsQueue.waitIdle();
    m_device.freeCommandBuffers(m_commandPool, commandBuffer);
}

}