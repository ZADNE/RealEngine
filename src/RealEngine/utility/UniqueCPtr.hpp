/**
 *  @author    Andreas Fertig (C++ Insights: Lambdas in unevaluated contexts)
 */
#pragma once
#include <memory>

namespace re {

namespace details {
/**
 * @todo Use lambda instead of this once GCC in CI can handle it.
 */
template<auto DeleteFn>
struct UniqueCPtrDeleter {
    template<class T>
    void operator()(T* ptr) const {
        DeleteFn(ptr);
    }
};
} // namespace details

/**
 * @brief           Implements RAII principle for C-style pointer-to-opaque objects
 * @tparam T        The opaque type
 * @tparam DeleteFn A function that is used to delete T.
 */
template<typename T, auto DeleteFn>
using UniqueCPtr = std::unique_ptr<T, details::UniqueCPtrDeleter<DeleteFn>>;

/**
 * @brief           Implements RAII principle for an opaque handle that is not a pointer
 * @tparam Handle   The opaque handle (typically an alias to an integer type)
 * @tparam FreeFn   A function that is used to free the handle.
 */
template<typename Handle, auto FreeFn>
class UniqueCHandle {
public:
    explicit UniqueCHandle(Handle handle)
        : m_handle{handle} {}

    UniqueCHandle(const UniqueCHandle&)            = delete; ///< Noncopyable
    UniqueCHandle& operator=(const UniqueCHandle&) = delete; ///< Noncopyable

    UniqueCHandle(UniqueCHandle&&)            = delete;      ///< Nonmovable
    UniqueCHandle& operator=(UniqueCHandle&&) = delete;      ///< Nonmovable

    ~UniqueCHandle() { FreeFn(m_handle); }

    Handle get() const { return m_handle; }
private:
    Handle m_handle;
};

} // namespace re
