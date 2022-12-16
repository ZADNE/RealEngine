﻿/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/buffers/Buffer.hpp>

#include <cassert>

#include <RealEngine/utility/Error.hpp>


namespace RE {

using enum vk::MemoryPropertyFlagBits;
using enum vk::BufferUsageFlagBits;

constexpr auto HOST_MEM = eHostVisible | eHostCoherent;

Buffer::Buffer(size_t sizeInBytes, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memProperty, const void* data/* = nullptr*/) {
    BufferAndMemory main{};
    if (data && (memProperty & HOST_MEM) != HOST_MEM) {//If initial data are provided or stage is requested
        auto stage = createBufferAndMemory(sizeInBytes, eTransferSrc, HOST_MEM);
        main = createBufferAndMemory(sizeInBytes, usage | eTransferDst, memProperty);
        //Copy from data to staging buffer
        std::memcpy(s_device->mapMemory(stage.memory, 0, sizeInBytes), data, sizeInBytes);
        s_device->unmapMemory(stage.memory);
        //Copy from staging to final buffer
        copyBetweenBuffers(stage.buffer, main.buffer, vk::BufferCopy{0u, 0u, sizeInBytes});
        //Destruct the temporary stage
        s_device->destroyBuffer(stage.buffer);
        s_device->free(stage.memory);
    } else {//If no initial data are provided
        main = createBufferAndMemory(sizeInBytes, usage, memProperty);
    }
    m_memory = main.memory;
    m_buffer = main.buffer;
}

Buffer::Buffer(Buffer&& other) noexcept :
    m_memory(other.m_memory),
    m_buffer(other.m_buffer) {
    other.m_memory = nullptr;
    other.m_buffer = nullptr;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept {
    std::swap(m_memory, other.m_memory);
    std::swap(m_buffer, other.m_buffer);
    return *this;
}

Buffer::~Buffer() {
    s_device->destroyBuffer(m_buffer);
    s_device->free(m_memory);
}

void Buffer::unmap() const {
    s_device->unmapMemory(m_memory);
}

void Buffer::bindAsVertexBuffer(uint32_t binding, uint64_t offsetInBytes) const {
    s_commandBuffer->bindVertexBuffers(binding, m_buffer, offsetInBytes);
}

void Buffer::bindAsIndexBuffer(uint64_t offsetInBytes, vk::IndexType indexType) const {
    s_commandBuffer->bindIndexBuffer(m_buffer, offsetInBytes, indexType);
}

uint32_t Buffer::selectMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const {
    auto memProperties = s_physicalDevice->getMemoryProperties2().memoryProperties;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw Exception{"Could not find memory that suits the buffer!"};
}

Buffer::BufferAndMemory Buffer::createBufferAndMemory(size_t sizeInBytes, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) const {
    vk::BufferCreateInfo createInfo{{},
        sizeInBytes,
        usage,
        vk::SharingMode::eExclusive
    };
    auto buffer = s_device->createBuffer(createInfo);
    auto memReq = s_device->getBufferMemoryRequirements2({buffer}).memoryRequirements;
    auto memory = s_device->allocateMemory({
        std::max(sizeInBytes, memReq.alignment),
        selectMemoryType(memReq.memoryTypeBits, properties)
        });
    s_device->bindBufferMemory2(vk::BindBufferMemoryInfo{buffer, memory, 0u});
    return BufferAndMemory{.buffer = buffer, .memory = memory};
}

void Buffer::copyBetweenBuffers(const vk::Buffer& src, const vk::Buffer& dst, const vk::BufferCopy& copyInfo) const {
    //Allocate temporary command buffer
    auto commandBuffer = s_device->allocateCommandBuffers({
        *s_commandPool, vk::CommandBufferLevel::ePrimary, 1u
    }).front();
    //Record the copy
    commandBuffer.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    commandBuffer.copyBuffer(src, dst, copyInfo);
    commandBuffer.end();
    //Submit the copy
    s_graphicsQueue->submit(vk::SubmitInfo{nullptr, {}, commandBuffer});
    //Free the temporary buffer
    s_graphicsQueue->waitIdle();
    s_device->freeCommandBuffers(*s_commandPool, commandBuffer);
}

void* Buffer::map(size_t offsetInBytes, size_t lengthInBytes) const {
    return s_device->mapMemory(m_memory, offsetInBytes, lengthInBytes);
}

}