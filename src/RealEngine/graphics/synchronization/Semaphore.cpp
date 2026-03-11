/**
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/synchronization/Semaphore.hpp>

namespace re {

Semaphore::Semaphore(const SemaphoreCreateInfo& createInfo)
    : m_semaphore(
          device().createSemaphore(
              vk::StructureChain{vk::SemaphoreCreateInfo{}, createInfo.type}
                  .get<vk::SemaphoreCreateInfo>()
          )
      ) {
    setDebugUtilsObjectName(m_semaphore, createInfo.debugName);
}

Semaphore::Semaphore(Semaphore&& other) noexcept
    : m_semaphore(std::exchange(other.m_semaphore, nullptr)) {
}

Semaphore& Semaphore::operator=(Semaphore&& other) noexcept {
    std::swap(m_semaphore, other.m_semaphore);
    return *this;
}

Semaphore::~Semaphore() {
    deletionQueue().enqueueDeletion(m_semaphore);
}

vk::Result Semaphore::wait(uint64_t waitForValue, uint64_t timeout /* = k_maxTimeout*/) {
    return device().waitSemaphores(
        vk::SemaphoreWaitInfo{{}, m_semaphore, waitForValue}, timeout
    );
}

} // namespace re
