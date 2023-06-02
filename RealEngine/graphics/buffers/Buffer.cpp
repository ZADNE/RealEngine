/*!
 *  @author    Dubsky Tomas
 */
#include <cassert>

#include <RealEngine/graphics/CommandBuffer.hpp>
#include <RealEngine/graphics/buffers/Buffer.hpp>
#include <RealEngine/utility/Error.hpp>

namespace re {

using enum vk::BufferUsageFlagBits;
using enum vma::AllocationCreateFlagBits;
using enum vma::MemoryUsage;

constexpr auto k_hostAccess = eHostAccessRandom | eHostAccessSequentialWrite;

Buffer::Buffer(const BufferCreateInfo& createInfo, void** pointerToMapped) {
    // If initial data are provided but data cannot be copied directly to the
    // main buffer
    if (createInfo.initData && !(createInfo.allocFlags & k_hostAccess)) {
        void* stageMapped = nullptr;
        // Create temporary stage buffer
        auto stage = createBufferAndAllocation(
            BufferCreateInfo{
                .allocFlags  = eHostAccessSequentialWrite | eMapped,
                .memoryUsage = eAutoPreferHost,
                .sizeInBytes = createInfo.sizeInBytes,
                .usage       = eTransferSrc},
            &stageMapped
        );
        // Create the main buffer
        auto mainCreateInfo = createInfo;
        mainCreateInfo.usage |= eTransferDst;
        std::tie(m_buffer, m_allocation) = createBufferAndAllocation(
            mainCreateInfo, pointerToMapped
        );
        // Copy data to staging buffer
        std::memcpy(stageMapped, createInfo.initData, createInfo.sizeInBytes);
        // Copy from staging to main buffer
        CommandBuffer::doOneTimeSubmit(
            [&](const vk::CommandBuffer& commandBuffer) {
                commandBuffer.copyBuffer(
                    stage.first,
                    m_buffer,
                    vk::BufferCopy{0u, 0u, createInfo.sizeInBytes}
                );
            }
        );
        // Destroy the temporary stage
        allocator().destroyBuffer(stage.first, stage.second);
    } else { // Stage is not required
        std::tie(m_buffer, m_allocation) = createBufferAndAllocation(
            createInfo, pointerToMapped
        );
    }
}

Buffer::Buffer(Buffer&& other) noexcept
    : m_allocation(other.m_allocation)
    , m_buffer(other.m_buffer) {
    other.m_allocation = nullptr;
    other.m_buffer     = nullptr;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept {
    std::swap(m_allocation, other.m_allocation);
    std::swap(m_buffer, other.m_buffer);
    return *this;
}

Buffer::~Buffer() {
    deletionQueue().enqueueDeletion(m_buffer);
    deletionQueue().enqueueDeletion(m_allocation);
}

std::pair<vk::Buffer, vma::Allocation> Buffer::createBufferAndAllocation(
    const BufferCreateInfo& createInfo, void** pointerToMapped
) const {
    vma::AllocationInfo  allocInfo;
    vk::BufferCreateInfo bufCreateInfo{
        {},
        createInfo.sizeInBytes,
        createInfo.usage,
        vk::SharingMode::eExclusive};
    vma::AllocationCreateInfo allocCreateInfo{
        createInfo.allocFlags, createInfo.memoryUsage};
    auto pair = allocator().createBuffer(
        bufCreateInfo, allocCreateInfo, allocInfo
    );
    if (pointerToMapped) {
        *pointerToMapped = allocInfo.pMappedData;
    }
    return pair;
}

} // namespace re