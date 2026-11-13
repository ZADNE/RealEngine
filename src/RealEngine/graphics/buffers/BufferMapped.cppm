/**
 *  @author    Dubsky Tomas
 */
export module realengine:graphics.buffers.BufferMapped;
import :graphics.buffers.Buffer;

export namespace re {

/**
 * @brief Is a buffer that is persistently mapped to CPU address space
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

private:
    BufferMapped(const BufferCreateInfo& createInfo, void* holder)
        : Buffer(createInfo, &holder)
        , m_mapped(reinterpret_cast<T*>(holder)) {}
    T* m_mapped{};
};

} // namespace re
