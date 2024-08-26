/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <concepts>
#include <utility>

namespace re {

/**
 * @brief       Implements RAII for C-style pointer-to-opaque objects
 * @tparam T    The opaque type
 * @tparam Deleter The function that will be used to delete the object
 */
template<typename T, std::invocable<T*> Deleter>
class RAIIWrapper {
public:
    RAIIWrapper(T* val)
        : m_val(val) {}

    RAIIWrapper(const RAIIWrapper&)            = delete;  /**< Noncopyable */
    RAIIWrapper& operator=(const RAIIWrapper&) = delete;  /**< Noncopyable */

    RAIIWrapper(RAIIWrapper&& other) noexcept;            /**< Movable */
    RAIIWrapper& operator=(RAIIWrapper&& other) noexcept; /**< Movable */

    ~RAIIWrapper() { Deleter(m_val); }

    operator T*() const { return m_val; } /**< Implicitly convertible */

    T* operator->() const { return m_val; }

private:
    T* m_val{};
};

template<typename T, std::invocable<T*> Deleter>
inline RAIIWrapper<T, Deleter>::RAIIWrapper(RAIIWrapper<T, Deleter>&& other) noexcept
    : m_val(std::exchange(other.m_val, nullptr)) {
}

template<typename T, std::invocable<T*> Deleter>
inline RAIIWrapper<T, Deleter>& RAIIWrapper<T, Deleter>::operator=(
    RAIIWrapper<T, Deleter>&& other
) noexcept {
    m_val = std::exchange(other.m_val, nullptr);
    return *this;
}

} // namespace re
