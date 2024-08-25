/*!
 *  @author    Dubsky Tomas
 */
#include <cassert>

#include <RealEngine/graphics/buffers/Buffer.hpp>
#include <RealEngine/graphics/commands/CommandBuffer.hpp>
#include <RealEngine/utility/Error.hpp>

using enum vk::BufferUsageFlagBits;
using enum vma::AllocationCreateFlagBits;
using enum vma::MemoryUsage;

namespace re {

constexpr auto k_hostAccess = eHostAccessRandom | eHostAccessSequentialWrite;

Buffer::Buffer(const BufferCreateInfo& createInfo, void** pointerToMapped) {
    assert(
        createInfo.sizeInBytes >=
        createInfo.initData.size_bytes() + createInfo.initDataDstOffset
    );
    // If initial data are provided but it cannot be copied directly to the main buffer
    if (!createInfo.initData.empty() && !(createInfo.allocFlags & k_hostAccess)) {
        void* stageMapped = nullptr;
        // Create temporary stage buffer
        auto stage = allocateBuffer(
            BufferCreateInfo{
                .allocFlags  = eHostAccessSequentialWrite | eMapped,
                .memoryUsage = eAutoPreferHost,
                .sizeInBytes = createInfo.initData.size_bytes(),
                .usage       = eTransferSrc
            },
            &stageMapped
        );
        // Create the main buffer
        auto mainCreateInfo = createInfo;
        mainCreateInfo.usage |= eTransferDst;
        std::tie(m_buffer, m_allocation) =
            allocateBuffer(mainCreateInfo, pointerToMapped);
        // Copy data to staging buffer
        std::memcpy(
            stageMapped, createInfo.initData.data(),
            createInfo.initData.size_bytes()
        );
        // Copy from staging to main buffer
        CommandBuffer::doOneTimeSubmit([&](const CommandBuffer& cb) {
            cb->copyBuffer(
                stage.first, m_buffer,
                vk::BufferCopy{
                    0u, createInfo.initDataDstOffset,
                    createInfo.initData.size_bytes()
                }
            );
        });
        // Destroy the temporary stage
        allocator().destroyBuffer(stage.first, stage.second);
    } else { // Stage is not required
        std::tie(m_buffer, m_allocation) = allocateBuffer(createInfo, pointerToMapped);
        if (!createInfo.initData.empty()) {
            // Copy init data to directly to the buffer
            std::byte* dst = reinterpret_cast<std::byte*>(*pointerToMapped) +
                             createInfo.initDataDstOffset;
            std::memcpy(
                dst, createInfo.initData.data(), createInfo.initData.size_bytes()
            );
        }
    }

    setDebugUtilsObjectName(m_buffer, createInfo.debugName);
}

Buffer::Buffer(Buffer&& other) noexcept
    : m_allocation(std::exchange(other.m_allocation, nullptr))
    , m_buffer(std::exchange(other.m_buffer, nullptr)) {
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

std::pair<vk::Buffer, vma::Allocation> Buffer::allocateBuffer(
    const BufferCreateInfo& createInfo, void** pointerToMapped
) const {
    vma::AllocationInfo allocInfo;
    vk::BufferCreateInfo bufCreateInfo{
        {}, createInfo.sizeInBytes, createInfo.usage, vk::SharingMode::eExclusive
    };
    vma::AllocationCreateInfo allocCreateInfo{
        createInfo.allocFlags, createInfo.memoryUsage
    };
    auto pair = allocator().createBuffer(bufCreateInfo, allocCreateInfo, allocInfo);
    if (pointerToMapped) {
        *pointerToMapped = allocInfo.pMappedData;
    }
    return pair;
}

} // namespace re
