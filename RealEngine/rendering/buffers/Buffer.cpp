/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/buffers/Buffer.hpp>

#include <cassert>

#include <RealEngine/rendering/CommandBuffer.hpp>
#include <RealEngine/utility/Error.hpp>


namespace RE {

using enum vk::MemoryPropertyFlagBits;
using enum vk::BufferUsageFlagBits;

constexpr auto HOST_MEM = eHostVisible | eHostCoherent;

Buffer::Buffer(vk::DeviceSize sizeInBytes, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memProperty, const void* data/* = nullptr*/) {
    BufferAndMemory main{};
    if (data && (memProperty & HOST_MEM) != HOST_MEM) {//If initial data are provided or stage is requested
        auto stage = createBufferAndMemory(sizeInBytes, eTransferSrc, HOST_MEM);
        main = createBufferAndMemory(sizeInBytes, usage | eTransferDst, memProperty);
        //Copy from data to staging buffer
        std::memcpy(s_device->mapMemory(stage.memory, 0, sizeInBytes), data, sizeInBytes);
        s_device->unmapMemory(stage.memory);
        //Copy from staging to final buffer
        CommandBuffer::doOneTimeSubmit([&](const vk::CommandBuffer& commandBuffer) {
            commandBuffer.copyBuffer(stage.buffer, main.buffer, vk::BufferCopy{0u, 0u, sizeInBytes});
        });
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

void Buffer::copyToBuffer(Buffer& dst, const vk::BufferCopy& info) const {
    RE::CommandBuffer::doOneTimeSubmit([&](const vk::CommandBuffer& commandBuffer) {
        commandBuffer.copyBuffer(m_buffer, dst.m_buffer, info);
    });
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

Buffer::BufferAndMemory Buffer::createBufferAndMemory(vk::DeviceSize sizeInBytes, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) const {
    vk::BufferCreateInfo createInfo{{},
        sizeInBytes,
        usage,
        vk::SharingMode::eExclusive
    };
    auto buffer = s_device->createBuffer(createInfo);
    auto memReq = s_device->getBufferMemoryRequirements2({buffer}).memoryRequirements;
    auto memory = s_device->allocateMemory({
        memReq.size,
        selectMemoryType(memReq.memoryTypeBits, properties)
        });
    s_device->bindBufferMemory2(vk::BindBufferMemoryInfo{buffer, memory, 0u});
    return BufferAndMemory{.buffer = buffer, .memory = memory};
}

void* Buffer::map(size_t offsetInBytes, size_t lengthInBytes) const {
    return s_device->mapMemory(m_memory, offsetInBytes, lengthInBytes);
}

}