/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <limits>

#include <RealEngine/renderer/DeletionQueue.hpp>
#include <RealEngine/renderer/VulkanObject.hpp>

namespace re {

/**
 * @brief Allows synchronization CPU<->GPU and also GPU<->GPU
 */
class Semaphore: public VulkanObject {
public:
    /**
     * @brief Creates binary unsignaled semaphore
     */
    Semaphore();

    /**
     * @brief Creates timeline semaphore with given initial time
     */
    Semaphore(uint64_t initialValue);

    Semaphore(const Semaphore&)            = delete;  /**< Noncopyable */
    Semaphore& operator=(const Semaphore&) = delete;  /**< Noncopyable */

    Semaphore(Semaphore&& other) noexcept;            /**< Movable */
    Semaphore& operator=(Semaphore&& other) noexcept; /**< Movable */

    ~Semaphore();

    static constexpr auto k_maxTimeout = std::numeric_limits<uint64_t>::max();

    /**
     * @brief Blocks the calling thread until the semaphore reaches the value
     */
    vk::Result wait(uint64_t waitForValue, uint64_t timeout = k_maxTimeout);

    const vk::Semaphore& operator*() const { return m_semaphore; }
    const vk::Semaphore* operator->() const { return &m_semaphore; }

    const vk::Semaphore& semaphore() const { return m_semaphore; }

protected:
    vk::Semaphore m_semaphore{};
};

static_assert(sizeof(Semaphore) == sizeof(vk::Semaphore));
static_assert(alignof(Semaphore) == alignof(vk::Semaphore));

} // namespace re
