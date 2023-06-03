/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <type_traits>
#include <vector>

#include <RealEngine/renderer/DeletionQueue.hpp>
#include <RealEngine/renderer/VulkanObject.hpp>

namespace re {

/**
 * @brief Specifies parameters for buffer creation
 */
struct BufferCreateInfo {
    // Memory-related
    vma::AllocationCreateFlags allocFlags  = {};
    vma::MemoryUsage           memoryUsage = vma::MemoryUsage::eAuto;
    // Buffer-related
    vk::DeviceSize       sizeInBytes = 0;
    vk::BufferUsageFlags usage       = {};
    const void*          initData    = nullptr;
};

/**
 * @brief Is a continuous block of memory
 * @note Use BufferMapped if you intend to map the buffer to CPU memory
 */
class Buffer: public VulkanObject {
public:
    Buffer(const BufferCreateInfo& createInfo)
        : Buffer(createInfo, nullptr) {}

    Buffer(const Buffer&)            = delete; /**< Noncopyable */
    Buffer& operator=(const Buffer&) = delete; /**< Noncopyable */

    Buffer(Buffer&& other) noexcept;            /**< Movable */
    Buffer& operator=(Buffer&& other) noexcept; /**< Movable */

    /**
     * @brief Frees the backing memory block on the GPU and destructs the
     * buffer.
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
    vk::Buffer      m_buffer{};
};

} // namespace re