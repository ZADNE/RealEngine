/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <memory>

namespace re {

/**
 * @brief           Implements RAII for C-style pointer-to-opaque objects
 * @tparam T        The opaque type
 * @tparam DeleteFn A function that is used to delete T.
 * @author          Andreas Fertig (C++ Insights: Lambdas in unevaluated contexts)
 */
template<typename T, auto DeleteFn>
using UniqueCPtr = std::unique_ptr<T, decltype([](T* obj) { DeleteFn(obj); })>;

} // namespace re
