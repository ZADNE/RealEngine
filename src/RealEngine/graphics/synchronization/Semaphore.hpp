/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <limits>

#include <RealEngine/renderer/DeletionQueue.hpp>
#include <RealEngine/renderer/ObjectUsingVulkan.hpp>

namespace re {

struct SemaphoreCreateInfo {
    vk::SemaphoreTypeCreateInfo type{vk::SemaphoreType::eBinary};

    // Debug
    [[no_unique_address]] DebugString<> debugName;
};

/**
 * @brief Allows synchronization CPU<->GPU and also GPU<->GPU
 */
class Semaphore: public ObjectUsingVulkan {
public:
    explicit Semaphore(const SemaphoreCreateInfo& createInfo);

    Semaphore(const Semaphore&)            = delete;  ///< Noncopyable
    Semaphore& operator=(const Semaphore&) = delete;  ///< Noncopyable

    Semaphore(Semaphore&& other) noexcept;            ///< Movable
    Semaphore& operator=(Semaphore&& other) noexcept; ///< Movable

    ~Semaphore();

    static constexpr auto k_maxTimeout = std::numeric_limits<uint64_t>::max();

    /**
     * @brief Blocks the calling thread until the semaphore reaches the value
     * @warning Supported only by timeline semaphores
     */
    vk::Result wait(uint64_t waitForValue, uint64_t timeout = k_maxTimeout);

    const vk::Semaphore& operator*() const { return m_semaphore; }
    const vk::Semaphore* operator->() const { return &m_semaphore; }

    const vk::Semaphore& semaphore() const { return m_semaphore; }

private:
    vk::Semaphore m_semaphore{};
};

static_assert(sizeof(Semaphore) == sizeof(vk::Semaphore));
static_assert(alignof(Semaphore) == alignof(vk::Semaphore));

} // namespace re
