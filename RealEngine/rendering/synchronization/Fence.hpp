/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/renderer/VulkanObject.hpp>
#include <RealEngine/renderer/DeletionQueue.hpp>


namespace RE {

/**
 * @brief Allows synchronization between GPU and CPU
*/
class Fence: public VulkanObject {
public:

    Fence(vk::FenceCreateFlags createFlags);

    Fence(const Fence&) = delete;                               /**< Noncopyable */
    Fence& operator=(const Fence&) = delete;                    /**< Noncopyable */

    Fence(Fence&& other) noexcept;                              /**< Movable */
    Fence& operator=(Fence&& other) noexcept;                   /**< Movable */

    ~Fence();

    static constexpr auto k_maxTimeout = std::numeric_limits<uint64_t>::max();

    /**
     * @brief Blocks the calling thread until the fence becomes signaled
    */
    vk::Result wait(uint64_t timeout = k_maxTimeout);

    /**
     * @brief Sets the fence unsignaled
    */
    void reset();

    const vk::Fence& operator*() const { return m_fence; }
    const vk::Fence* operator->() const { return &m_fence; }

    const vk::Fence& fence() const { return m_fence; }

protected:

    vk::Fence m_fence{};
};

static_assert(sizeof(Fence) == sizeof(vk::Fence));
static_assert(alignof(Fence) == alignof(vk::Fence));

}