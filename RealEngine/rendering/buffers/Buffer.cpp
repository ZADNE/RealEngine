﻿/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/buffers/Buffer.hpp>

#include <cassert>

#include <RealEngine/rendering/CommandBuffer.hpp>
#include <RealEngine/utility/Error.hpp>


namespace RE {

using enum vk::MemoryPropertyFlagBits;
using enum vk::BufferUsageFlagBits;

constexpr auto k_hostMem = eHostVisible | eHostCoherent;

Buffer::Buffer(vk::DeviceSize sizeInBytes, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memProperty, const void* data/* = nullptr*/) {
    BufferAndMemory main{};
    if (data && (memProperty & k_hostMem) != k_hostMem) {//If initial data are provided or stage is requested
        auto stage = createBufferAndMemory(sizeInBytes, eTransferSrc, k_hostMem);
        main = createBufferAndMemory(sizeInBytes, usage | eTransferDst, memProperty);
        //Copy from data to staging buffer
        std::memcpy(device().mapMemory(stage.memory, 0, sizeInBytes), data, sizeInBytes);
        device().unmapMemory(stage.memory);
        //Copy from staging to final buffer
        CommandBuffer::doOneTimeSubmit([&](const vk::CommandBuffer& commandBuffer) {
            commandBuffer.copyBuffer(stage.buffer, main.buffer, vk::BufferCopy{0u, 0u, sizeInBytes});
        });
        //Destruct the temporary stage
        device().destroyBuffer(stage.buffer);
        device().free(stage.memory);
    } else {//If no initial data are provided
        main = createBufferAndMemory(sizeInBytes, usage, memProperty);
    }
    m_memory = main.memory;
    m_buffer = main.buffer;
}

Buffer::Buffer(Buffer&& other) noexcept:
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
    deletionQueue().enqueueDeletion(m_buffer);
    deletionQueue().enqueueDeletion(m_memory);
}

void Buffer::unmap() const {
    device().unmapMemory(m_memory);
}

uint32_t Buffer::selectMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const {
    auto memProperties = physicalDevice().getMemoryProperties2().memoryProperties;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw Exception{"Could not find memory that suits the buffer!"};
}

Buffer::BufferAndMemory Buffer::createBufferAndMemory(vk::DeviceSize sizeInBytes, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) const {
    vk::BufferCreateInfo createInfo{{},
        sizeInBytes,
        usage,
        vk::SharingMode::eExclusive
    };
    auto buffer = device().createBuffer(createInfo);
    auto memReq = device().getBufferMemoryRequirements2({buffer}).memoryRequirements;
    auto memory = device().allocateMemory({
        memReq.size,
        selectMemoryType(memReq.memoryTypeBits, properties)
    });
    device().bindBufferMemory2(vk::BindBufferMemoryInfo{buffer, memory, 0u});
    return BufferAndMemory{.buffer = buffer, .memory = memory};
}

void* Buffer::map(size_t offsetInBytes, size_t lengthInBytes) const {
    return device().mapMemory(m_memory, offsetInBytes, lengthInBytes);
}

}