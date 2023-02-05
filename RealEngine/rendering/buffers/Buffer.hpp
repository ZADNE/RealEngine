/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vector>
#include <type_traits>

#include <RealEngine/renderer/VulkanObject.hpp>
#include <RealEngine/renderer/VulkanDeletionQueue.hpp>


namespace RE {

/**
 * @brief Is a continuous block of memory stored in the GPU's memory
*/
class Buffer: public VulkanObject {
public:

    /**
     * @brief Constructs buffer of given size, usage and data
     * @param sizeInBytes Size in bytes of the buffer
     * @param usage Restrict usage of the buffer
     * @param data  If a valid pointer is provided, it is used to initialize the contents of the buffer.
     *              If the nullptr is provided, the contents of the buffer are undefined.
    */
    Buffer(vk::DeviceSize sizeInBytes, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memProperty, const void* data = nullptr);

    template<typename T, typename = std::enable_if_t<!std::is_pointer_v<T>>>
    Buffer(vk::DeviceSize sizeInBytes, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memProperty, const T& data) :
        Buffer(sizeInBytes, usage, memProperty, &data) {}

    Buffer(const Buffer&) = delete;
    Buffer(Buffer&& other) noexcept;

    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&& other) noexcept;

    /**
     * @brief Frees the backing memory block on the GPU and destructs the buffer.
    */
    ~Buffer();

    /**
     * @brief Maps a range of the buffer to the client's memory
     * @tparam T Reinterpreted type of the returned pointer
    */
    template<typename T>
    T* map(size_t offsetInBytes, size_t lengthInBytes) const {
        return reinterpret_cast<T*>(map(offsetInBytes, lengthInBytes));
    }

    /**
     * @brief Releases the mapping of the buffer
     * @return True if success. Buffer's contents are undefined if false is returned.
    */
    void unmap() const;

    void copyToBuffer(Buffer& dst, const vk::BufferCopy& info) const;

    const vk::Buffer& operator*() const { return m_buffer; }
    const vk::Buffer* operator->() const { return &m_buffer; }

    const vk::Buffer& buffer() const { return m_buffer; }

protected:

    void* map(size_t offsetInBytes, size_t lengthInBytes) const;

    uint32_t selectMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) const;
    struct BufferAndMemory {
        vk::Buffer buffer = nullptr;
        vk::DeviceMemory memory = nullptr;
    };
    BufferAndMemory createBufferAndMemory(vk::DeviceSize sizeInBytes, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) const;

    vk::DeviceMemory m_memory{};
    vk::Buffer m_buffer{};
};

}