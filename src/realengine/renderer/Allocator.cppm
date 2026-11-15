/**
 *  @author    Dubsky Tomas
 */
export module realengine:renderer.Allocator;
import vk_mem_alloc;

export namespace re {

/**
 * @brief Is a simple RAII wrapper around vma::Allocator
 */
class Allocator: public vma::Allocator {
public:
    /**
     * @brief Constructible only by moving a vma::Allocator in
     */
    Allocator(vma::Allocator&& other) noexcept
        : vma::Allocator(std::move(other)) {}

    Allocator(const Allocator&)            = delete; ///< Noncopyable
    Allocator& operator=(const Allocator&) = delete; ///< Noncopyable

    Allocator(Allocator&&)            = delete;      ///< Nonmovable
    Allocator& operator=(Allocator&&) = delete;      ///< Nonmovable

    ~Allocator() { destroy(); }
};

} // namespace re
