/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/graphics/buffers/Buffer.hpp>

namespace re {

/**
 * @brief Is buffer that is persistently mapped throughout its lifetime
 * @tparam T The type that this buffer represents
 */
template<typename T = std::byte>
class BufferMapped: public Buffer {
public:
    /**
     * @brief Creates and mappes the buffer to CPU
     * @warning It is your job to unsure that the createInfo::allocFlags
     * contains flag eMapped
     */
    BufferMapped(const BufferCreateInfo& createInfo)
        : Buffer(createInfo, reinterpret_cast<void**>(&m_mapped)) {}

    const T& operator[](auto i) const { return m_mapped[i]; }
    T&       operator[](auto i) { return m_mapped[i]; }
    const T& operator*() const { return *m_mapped; }
    T&       operator*() { return *m_mapped; }
    const T* operator->() const { return m_mapped; }
    T*       operator->() { return m_mapped; }

    const T* mapped() const { return m_mapped; }
    T*       mapped() { return m_mapped; }

protected:
    T* m_mapped;
};

} // namespace re