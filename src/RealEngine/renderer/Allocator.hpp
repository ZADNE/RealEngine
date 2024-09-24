/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vma/vk_mem_alloc.hpp>

namespace re {

/**
 * @brief Is a simple RAII wrapper around vma::Allocator
 */
class Allocator: public vma::Allocator {
public:
    /**
     * @brief Constructible only by moving a vma::Allocator in
     */
    Allocator(vma::Allocator&& other) noexcept
        : vma::Allocator(other) {}

    Allocator(const Allocator&)            = delete; ///< Noncopyable
    Allocator& operator=(const Allocator&) = delete; ///< Noncopyable

    Allocator& operator=(Allocator&&) = delete;      ///< Nonmovable

    ~Allocator() { destroy(); }
};

} // namespace re
