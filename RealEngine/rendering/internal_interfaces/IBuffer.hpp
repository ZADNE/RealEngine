/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <utility>

#include <RealEngine/rendering/buffers/types.hpp>

namespace RE {

enum class BufferStorage {
    IMMUTABLE,
    MUTABLE
};

enum class BufferAccessFrequency {
    STREAM = 0,
    STATIC = 1,
    DYNAMIC = 2
};

enum class BufferAccessNature {
    DRAW = 0,
    READ = 1,
    COPY = 2
};

enum class BufferUsageFlags : unsigned int {
    NO_FLAGS = 0,
    DYNAMIC_STORAGE = 1,
    MAP_READ = 2,
    MAP_WRITE = 4,
    MAP_PERSISTENT = 8,
    MAP_COHERENT = 16,
    CLIENT_STORAGE = 32
};

inline BufferUsageFlags operator|(BufferUsageFlags a, BufferUsageFlags b) {
    return static_cast<BufferUsageFlags>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline bool operator&(BufferUsageFlags a, BufferUsageFlags b) {
    return static_cast<unsigned int>(a) & static_cast<unsigned int>(b);
}

/**
 * @brief Restricts the usage of a buffer mapping
*/
enum class BufferMapUsageFlags : unsigned int {
    READ = 1,                   /**< Indicates that the mapping may be used to read buffer's data */
    WRITE = 2,                  /**< Indicates that the mapping may be used to modify buffer's data */
    PERSISTENT = 4,             /**< Indicates that the client intends to hold and use the mapping during subsequent GL operation */
    COHERENT = 8,               /**< Indicates that a persistent mapping is also to be coherent */
    INVALIDATE_RANGE = 16,      /**< Indicates that the previous contents of the specified range may be discarded */
    INVALIDATE_BUFFER = 32,     /**< Indicates that the previous contents of the entire buffer may be discarded */
    FLUSH_EXPLICIT = 64,        /**< Indicates that one or more discrete subranges of the mapping may be modified */
    UNSYNCHRONIZED = 128        /**< Indicates that the GL should not attempt to synchronize pending operations on the buffer */
};

inline BufferMapUsageFlags operator|(BufferMapUsageFlags a, BufferMapUsageFlags b) {
    return static_cast<BufferMapUsageFlags>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

inline bool operator&(BufferMapUsageFlags a, BufferMapUsageFlags b) {
    return static_cast<unsigned int>(a) & static_cast<unsigned int>(b);
}

/**
 * @brief Contains all members of Buffer
 * @note For internal usage in RealEngine.
*/
class BufferInternals {
    friend class GL46_Buffer;
    friend class GL46_VertexArray;
public:

    ~BufferInternals() = default;

    BufferInternals(const BufferInternals&) = delete;
    BufferInternals(BufferInternals&& other) noexcept :
        m_id(other.m_id),
        m_sizeInBytes(other.m_sizeInBytes),
        m_access(other.m_access)
    #ifndef _DEBUG
    {
    #else
        , m_storage(other.m_storage) {
    #endif // _DEBUG
        other.m_id = 0;
    }

    BufferInternals& operator=(const BufferInternals&) = delete;
    BufferInternals& operator=(BufferInternals && other) noexcept {
        std::swap(m_id, other.m_id);
        m_sizeInBytes = other.m_sizeInBytes;
        m_access = other.m_access;
    #ifdef _DEBUG
        m_storage = other.m_storage;
    #endif // _DEBUG
        return *this;
    }

    size_t size() const { return m_sizeInBytes; }

private:

#ifndef _DEBUG
    BufferInternals(unsigned int id, size_t sizeInBytes, unsigned int access) :
        m_id(id), m_sizeInBytes(sizeInBytes), m_access(access) {}
#else
    BufferInternals(unsigned int id, size_t sizeInBytes, unsigned int access, BufferStorage storage) :
        m_id(id), m_sizeInBytes(sizeInBytes), m_access(access), m_storage(storage) {}
#endif

    unsigned int m_id = 0;      /**< Internal identifier */
    size_t m_sizeInBytes = 0;   /**< Size of the buffer */
    unsigned int m_access = 0;  /**< Access hints of the buffer; relevant only for mutable buffers */

#ifdef _DEBUG
    BufferStorage m_storage = BufferStorage::IMMUTABLE;
#endif // _DEBUG
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

    virtual BufferInternals constructImmutable(size_t sizeInBytes, BufferUsageFlags flags, const void* data) const = 0;
    virtual BufferInternals constructMutable(size_t sizeInBytes, BufferAccessFrequency accessFreq, BufferAccessNature accessNature, const void* data) const = 0;
    virtual void destruct(BufferInternals& bf) const = 0;

    virtual void bind(const BufferInternals& bf, BufferType bindType) const = 0;
    virtual void bindIndexed(const BufferInternals& bf, const BufferTypedIndex& index) const = 0;

    virtual void overwrite(const BufferInternals& bf, size_t offsetInBytes, size_t countBytes, const void* data) const = 0;

    virtual void redefine(BufferInternals& bf, size_t sizeInBytes, const void* data) const = 0;

    virtual void invalidate(const BufferInternals& bf) const = 0;
    virtual void invalidate(const BufferInternals& bf, size_t lengthInBytes) const = 0;

    virtual void* map(const BufferInternals& bf, size_t offsetInBytes, size_t lengthInBytes, BufferMapUsageFlags mappingUsage) const = 0;
    virtual void flushMapped(const BufferInternals& bf, size_t offsetInBytes, size_t lengthInBytes) const = 0;
    virtual bool unmap(const BufferInternals& bf) const = 0;
};

}