/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vector>

#include <RealEngine/rendering/internal_interfaces/IBuffer.hpp>
#include <RealEngine/rendering/Renderer.hpp>


namespace RE {

template<Renderer> class VertexArray;

/**
 * @brief Is a continuous block of memory stored in the GPU's memory
 * @tparam R The renderer that will perform the commands
*/
template<Renderer R = RendererLateBind>
class Buffer {
    friend class VK13Fixture;
    friend class VertexArray<R>;
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
     * @brief Binds the buffer to a generic binding point.
     * @details Each type of buffer has its own generic binding point.
     * @param bindType The type of binding point to bind this buffer to.
    */
    //void bind(BufferType bindType) const;

    /**
     * @brief Binds the buffer to an indexed binding point (as well as generic binding point)
     * @details Indexed binding is only available for these types:
     * ATOMIC_COUNTER, TRANSFORM_FEEDBACK, UNIFORM, and SHADER_STORAGE.
     * @param index The typed index to bind this buffer to.
    */
    //void bindIndexed(const BufferTypedIndex& index) const;

    /**
     * @brief Overwrites a portion of or whole buffer
     * @param offsetInBytes Offset in bytes within the buffer to overwrite
     * @param countBytes Number of bytes to overwrite
     * @param data The data to overwrite with
    */
    //void overwrite(size_t offsetInBytes, size_t countBytes, const void* data) const;

    /**
     * @brief Overwrites the buffer with instance of a type
     * @tparam T The type to overwrite the buffer with
     * @param offsetInBytes Offset in bytes within the buffer to overwrite
     * @param data The instance to overwrite the buffer with
    */
    /*template<typename T>
    void overwrite(size_t offsetInBytes, const T& data) const {
        overwrite(offsetInBytes, sizeof(T), reinterpret_cast<const void*>(&data));
    }*/

    /**
     * @brief Overwrites the buffer with an array
     * @tparam T Element type of the array
     * @param offsetInBytes Offset in bytes within the buffer to overwrite
     * @param data Array (provided as a vector) to overwrite the buffer with
    */
    /*template<typename T>
    void overwrite(size_t offsetInBytes, const std::vector<T>& data) const {
        overwrite(offsetInBytes, data.size() * sizeof(T), reinterpret_cast<const void*>(data.data()));
    }*/

    /**
     * @brief Ensures that the buffer is big enough to store the data and writes it
     *
     * Since this operation may cause resize of the buffer,
     * it can only be applied to mutable buffers.
     * @param sizeInBytes Size of the data required to write
     * @param data The data to write
    */
    //void redefine(size_t sizeInBytes, const void* data);

    /**
     * @brief Ensures that the buffer is big enough to store the data and writes it
     *
     * Since this operation may cause resize of the buffer,
     * it can only be applied to mutable buffers.
     * @tparam T Type to write
     * @param data Instance to write
    */
    /*template<typename T>
    void redefine(const T& data) {
        redefine(sizeof(T), reinterpret_cast<const void*>(&data));
    }*/

    /**
     * @brief Ensures that the buffer is big enough to store the data and writes it
     *
     * Since this operation may cause resize of the buffer,
     * it can only be applied to mutable buffers.
     * @tparam T Element type of the array
     * @param data Array (provided as a vector) to redefine the buffer as
    */
    /*template<typename T>
    void redefine(const std::vector<T>& data) {
        redefine(data.size() * sizeof(T), reinterpret_cast<const void*>(data.data()));
    }*/

    /**
     * @brief Makes the contents of the buffer undefined
     *
     * This effectively frees the backing block of memory of the buffer.
    */
    //void invalidate() const;

    /**
     * @brief Makes a range of the buffer undefined
     * @param lengthInBytes Length of the range at the beginning of the buffer to invalidate
    */
    //void invalidate(size_t lengthInBytes) const;

    /**
     * @brief Maps a range of the buffer to the client's memory
     * @tparam T Reinterpreted type of the returned pointer
    */
    template<typename T>
    T* map(size_t offsetInBytes, size_t lengthInBytes) const {
        return reinterpret_cast<T*>(map(offsetInBytes, lengthInBytes));
    }

    /**
     * @brief Indicates modifications to a mapped range of the buffer
    */
    //void flushMapped(size_t offsetInBytes, size_t lengthInBytes) const;

    /**
     * @brief Releases the mapping of the buffer
     * @return True if success. Buffer's contents are undefined if false is returned.
    */
    void unmap() const;

    /**
     * @brief Gets size of the buffer in bytes
    */
    //size_t size() const;

protected:

    void* map(size_t offsetInBytes, size_t lengthInBytes) const;

    BufferID m_id;
    size_t m_sizeInBytes = 0;   /**< Size of the buffer */

    static inline R::Buffer* s_impl = nullptr;
};

}