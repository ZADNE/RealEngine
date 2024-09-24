/**
 *  @author    Dubsky Tomas
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
 * @brief           Implements RAII for C-style pointer-to-opaque objects
 * @tparam T        The opaque type
 * @tparam DeleteFn A function that is used to delete T.
 * @author          Andreas Fertig (C++ Insights: Lambdas in unevaluated contexts)
 */
template<typename T, auto DeleteFn>
using UniqueCPtr = std::unique_ptr<T, details::UniqueCPtrDeleter<DeleteFn>>;

} // namespace re
