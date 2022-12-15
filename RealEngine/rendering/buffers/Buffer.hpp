/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vector>

#include <RealEngine/rendering/internal_interfaces/IBuffer.hpp>
#include <RealEngine/rendering/Renderer.hpp>


namespace RE {

template<Renderer> class DescriptorSet;

/**
 * @brief Is a continuous block of memory stored in the GPU's memory
 * @tparam R The renderer that will perform the commands
*/
template<Renderer R = RendererLateBind>
class Buffer {
    friend class VK13Fixture;
    friend class DescriptorSet<R>;
public:

    /**
     * @brief Constructs buffer of given size, usage and data
     * @param sizeInBytes Size in bytes of the buffer
     * @param usage Restrict usage of the buffer
     * @param data  If a valid pointer is provided, it is used to initialize the contents of the buffer.
     *              If the nullptr is provided, the contents of the buffer are undefined.
    */
    Buffer(size_t sizeInBytes, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memProperty, const void* data = nullptr);

    /**
     * @brief Contruct buffer as a storage for given type
     * @tparam T The type that the buffer will store (used to determine buffer's size)
     * @param usage Restrict usage of the buffer
     * @param data The instance of the type that the buffer will be initialized with
    */
    template<typename T>
    Buffer(vk::BufferUsageFlags usage, const T& data) :
        Buffer(sizeof(T), usage, &data) {}

    /**
     * @brief Contructs buffer as an array
     * @tparam T Element type of the array
     * @param usage Restrict usage of the buffer
     * @param data Array (provided as a vector) that determines the size and initial contents of the buffer
    */
    template<typename T>
    Buffer(vk::BufferUsageFlags usage, const std::vector<T>& data) :
        Buffer(data.size() * sizeof(T), usage, data.data()) {}

    Buffer(const Buffer<R>&) = delete;
    Buffer(Buffer<R>&& other) noexcept;

    Buffer<R>& operator=(const Buffer<R>&) = delete;
    Buffer<R>& operator=(Buffer<R>&& other) noexcept;

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

    void bindAsVertexBuffer(uint32_t binding, uint64_t offsetInBytes) const;

    void bindAsIndexBuffer(uint64_t offsetInBytes, vk::IndexType indexType) const;

protected:

    void* map(size_t offsetInBytes, size_t lengthInBytes) const;

    BufferID m_id;
    size_t m_sizeInBytes = 0;   /**< Size of the buffer */

    static inline R::Buffer* s_impl = nullptr;
};

}