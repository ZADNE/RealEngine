/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <cstddef>
#include <utility>

#include <vulkan/vulkan.hpp>

namespace RE {

/**
 * @brief Contains all members of Buffer
 * @note For internal usage in RealEngine.
*/
class BufferID {
    friend class VK13Buffer;
public:

    ~BufferID() = default;

    BufferID(const BufferID&) = delete;
    BufferID(BufferID&& other) noexcept :
        m_(other.m_) {
        std::memset(&other, 0, sizeof(other));
    }

    BufferID& operator=(const BufferID&) = delete;
    BufferID& operator=(BufferID&& other) noexcept {
        std::swap(m_, other.m_);
        return *this;
    }

private:

    struct VK13 {
        vk::Buffer buffer;
    };

    BufferID(const VK13& vk13) :
        m_({.vk13 = vk13}) {}

    union {
        VK13 vk13;
    } m_;
};

/**
* @brief Is a renderer-agnostic interface to Buffer's implementation.
*
* This is used internally by Buffer.
*
* @see RE::Buffer
*/
class IBuffer {
public:

    virtual BufferID construct(size_t sizeInBytes, vk::BufferUsageFlags flags, const void* data) const = 0;
    virtual void destruct(BufferID& bf) const = 0;

    /*virtual void bind(const BufferID& bf, BufferType bindType) const = 0;
    virtual void bindIndexed(const BufferID& bf, const BufferTypedIndex& index) const = 0;

    virtual void overwrite(const BufferID& bf, size_t offsetInBytes, size_t countBytes, const void* data) const = 0;

    virtual void redefine(BufferID& bf, size_t sizeInBytes, const void* data) const = 0;

    virtual void invalidate(const BufferID& bf) const = 0;
    virtual void invalidate(const BufferID& bf, size_t lengthInBytes) const = 0;

    virtual void* map(const BufferID& bf, size_t offsetInBytes, size_t lengthInBytes, BufferMapUsageFlags mappingUsage) const = 0;
    virtual void flushMapped(const BufferID& bf, size_t offsetInBytes, size_t lengthInBytes) const = 0;
    virtual bool unmap(const BufferID& bf) const = 0;*/
};

}