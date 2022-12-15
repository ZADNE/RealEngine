﻿/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vulkan/vulkan.hpp>

namespace RE {

/**
 * @brief Contains all members of Buffer
 * @note For internal usage in RealEngine.
*/
class BufferID {
    friend class VK13Buffer;
    friend class VK13DescriptorSet;
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
        vk::Buffer mainBuffer = nullptr;
        vk::DeviceMemory mainMemory = nullptr;
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

    virtual BufferID construct(uint64_t sizeInBytes, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memProperty, const void* data) const = 0;
    virtual void destruct(BufferID& bf) const = 0;

    virtual void* map(const BufferID& bf, uint64_t offsetInBytes, uint64_t lengthInBytes) const = 0;
    virtual void unmap(const BufferID& bf) const = 0;

    virtual void bindAsVertexBuffer(const BufferID& bf, uint32_t binding, uint64_t offsetInBytes) const = 0;
    virtual void bindAsIndexBuffer(const BufferID& bf, uint64_t offsetInBytes, vk::IndexType indexType) const = 0;
};

}