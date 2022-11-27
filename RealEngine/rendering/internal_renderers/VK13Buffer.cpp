#include "VK13Buffer.hpp"
#include "VK13Buffer.hpp"
/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/VK13Buffer.hpp>

namespace RE {

using enum vk::MemoryPropertyFlagBits;
using enum vk::BufferUsageFlagBits;

BufferID VK13Buffer::construct(size_t sizeInBytes, vk::BufferUsageFlags usage, const void* data) const {
    if (data) {//If initial data are provided
        //Create staging and final buffer
        auto stage = createBufferAndMemory(sizeInBytes, eTransferSrc, eHostVisible | eHostCoherent);
        auto buffer = createBufferAndMemory(sizeInBytes, usage | eTransferDst, eDeviceLocal);
        //Copy from data to staging buffer
        std::memcpy(m_device.mapMemory(stage.m_.vk13.memory, 0, sizeInBytes), data, sizeInBytes);
        m_device.unmapMemory(stage.m_.vk13.memory);
        //Copy from staging to final buffer
        copyBetweenBuffers(stage.m_.vk13.buffer, buffer.m_.vk13.buffer, vk::BufferCopy{0u, 0u, sizeInBytes});
        //Destruct the staging buffer
        destruct(stage);
        //Return the final buffer
        return buffer;
    } else {//If no initial data are provided
        return createBufferAndMemory(sizeInBytes, usage, eDeviceLocal);
    }
}

void VK13Buffer::destruct(BufferID& bf) const {
    m_device.destroyBuffer(bf.m_.vk13.buffer);
    m_device.free(bf.m_.vk13.memory);
}

uint32_t VK13Buffer::selectMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const {
    auto memProperties = m_physicalDevice.getMemoryProperties2().memoryProperties;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    return 0;
}

BufferID VK13Buffer::createBufferAndMemory(size_t sizeInBytes, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) const {
    vk::BufferCreateInfo createInfo{{},
        sizeInBytes,
        usage,
        vk::SharingMode::eExclusive
    };
    auto buffer = m_device.createBuffer(createInfo);
    auto memReq = m_device.getBufferMemoryRequirements2({buffer}).memoryRequirements;
    auto memory = m_device.allocateMemory({
        sizeInBytes, selectMemoryType(memReq.memoryTypeBits, properties)
    });
    m_device.bindBufferMemory2(vk::BindBufferMemoryInfo{buffer, memory, 0u});
    return BufferID::VK13{.buffer = buffer, .memory = memory};
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