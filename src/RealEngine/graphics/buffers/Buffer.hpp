/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <memory>
#include <span>
#include <type_traits>
#include <vector>

#include <RealEngine/renderer/DeletionQueue.hpp>
#include <RealEngine/renderer/VulkanObject.hpp>

namespace re {

/**
 * @brief Converts an object to span of bytes
 */
template<class T>
std::span<const std::byte> objectToByteSpan(const T& t) {
    return {reinterpret_cast<const std::byte*>(std::addressof(t)), sizeof(T)};
}

/**
 * @brief Specifies parameters for buffer creation
 */
struct BufferCreateInfo {
    // Memory-related
    vma::AllocationCreateFlags allocFlags = {};
    vma::MemoryUsage memoryUsage          = vma::MemoryUsage::eAuto;

    // Buffer-related
    vk::DeviceSize sizeInBytes = 0;
    vk::BufferUsageFlags usage = {};

    // Initial data
    std::span<const std::byte> initData = {};
    vk::DeviceSize initDataDstOffset    = {
    }; // Offset in bytes where initData will be placed

    // Debug
    [[no_unique_address]] DebugName<> debugName;
};

/**
 * @brief Is a continuous block of memory allocated by Vulkan
 * @note Use BufferMapped if you intend to map the buffer to CPU memory
 */
class Buffer: public VulkanObject {
public:
    /**
     * @brief Constructs a null buffer without any backing memory
     */
    explicit Buffer() {}

    explicit Buffer(const BufferCreateInfo& createInfo)
        : Buffer(createInfo, nullptr) {}

    Buffer(const Buffer&)            = delete;  /**< Noncopyable */
    Buffer& operator=(const Buffer&) = delete;  /**< Noncopyable */

    Buffer(Buffer&& other) noexcept;            /**< Movable */
    Buffer& operator=(Buffer&& other) noexcept; /**< Movable */

    /**
     * @brief Frees the backing memory block on the GPU and destructs the buffer.
     */
    ~Buffer();

    const vk::Buffer& operator*() const { return m_buffer; }
    const vk::Buffer* operator->() const { return &m_buffer; }

    const vk::Buffer& buffer() const { return m_buffer; }

protected:
    /**
     * @param pointerToMapped Output paramter - It is used to return pointer to
     * the mapped buffer if persistent mapping is requested
     */
    Buffer(const BufferCreateInfo& createInfo, void** pointerToMapped);

    std::pair<vk::Buffer, vma::Allocation> allocateBuffer(
        const BufferCreateInfo& createInfo, void** pointerToMapped
    ) const;

    vma::Allocation m_allocation{};
    vk::Buffer m_buffer{};
};

} // namespace re
