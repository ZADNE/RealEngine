/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/graphics/buffers/Buffer.hpp>

namespace re {

/**
 * @brief Is a buffer that is persistently mapped throughout its lifetime
 * @tparam T The type that this buffer represents
 */
template<typename T = std::byte>
class BufferMapped: public Buffer {
public:
    /**
     * @brief Constructs a null buffer without any backing memory
     */
    explicit BufferMapped() {}

    /**
     * @brief Creates and mapps the buffer to CPU-accessible memory
     * @warning It is your job to unsure that the createInfo::allocFlags
     * contains flag eMapped
     */
    explicit BufferMapped(const BufferCreateInfo& createInfo)
        : BufferMapped(createInfo, nullptr) {}

    const T& operator[](auto i) const { return m_mapped[i]; }
    T& operator[](auto i) { return m_mapped[i]; }
    const T& operator*() const { return *m_mapped; }
    T& operator*() { return *m_mapped; }
    const T* operator->() const { return m_mapped; }
    T* operator->() { return m_mapped; }

    const T* mapped() const { return m_mapped; }
    T* mapped() { return m_mapped; }

protected:
    BufferMapped(const BufferCreateInfo& createInfo, void* holder)
        : Buffer(createInfo, &holder)
        , m_mapped(reinterpret_cast<T*>(holder)) {}
    T* m_mapped{};
};

} // namespace re
