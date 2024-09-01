/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <utility>

namespace re {

/**
 * @brief           Implements RAII for C-style pointer-to-opaque objects
 * @tparam T        The opaque type
 * @tparam Deleter  A functor that, when default constructed, is used to delete T.
 */
template<typename T, class Deleter>
class RAIIWrapperFntr {
public:
    RAIIWrapperFntr(T* val)
        : m_val(val) {}

    RAIIWrapperFntr(const RAIIWrapperFntr&) = delete; /**< Noncopyable */
    RAIIWrapperFntr& operator=(const RAIIWrapperFntr&) = delete; /**< Noncopyable */

    RAIIWrapperFntr(RAIIWrapperFntr&& other) noexcept;           /**< Movable */
    RAIIWrapperFntr& operator=(RAIIWrapperFntr&& other) noexcept; /**< Movable */

    ~RAIIWrapperFntr() { Deleter{}(m_val); }

    operator T*() const { return m_val; } /**< Implicitly convertible */

    T* operator->() const { return m_val; }

private:
    T* m_val{};
};

template<typename T, class Deleter>
inline RAIIWrapperFntr<T, Deleter>::RAIIWrapperFntr(RAIIWrapperFntr<T, Deleter>&& other
) noexcept
    : m_val(std::exchange(other.m_val, nullptr)) {
}

template<typename T, class Deleter>
inline RAIIWrapperFntr<T, Deleter>& RAIIWrapperFntr<T, Deleter>::operator=(
    RAIIWrapperFntr<T, Deleter>&& other
) noexcept {
    m_val = std::exchange(other.m_val, nullptr);
    return *this;
}

/**
 * @brief           Implements RAII for C-style pointer-to-opaque objects
 * @tparam T        The opaque type
 * @tparam DeleteFn The function that will be used to delete the object
 */
template<typename T, auto DeleteFn>
using RAIIWrapper = RAIIWrapperFntr<T, decltype([](T* obj) { DeleteFn(obj); })>;

} // namespace re
