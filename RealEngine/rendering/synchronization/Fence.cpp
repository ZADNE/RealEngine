/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/synchronization/Fence.hpp>


namespace RE {

Fence::Fence(vk::FenceCreateFlags createFlags):
    m_fence(device().createFence(vk::FenceCreateInfo{createFlags})) {
}

Fence::Fence(Fence&& other) noexcept:
    m_fence(other.m_fence) {
    other.m_fence = nullptr;
}

Fence& Fence::operator=(Fence&& other) noexcept {
    std::swap(m_fence, other.m_fence);
    return *this;
}

Fence::~Fence() {
    deletionQueue().enqueueDeletion(m_fence);
}

vk::Result Fence::wait(uint64_t timeout/* = k_maxTimeout*/) {
    return device().waitForFences(m_fence, true, timeout);
}

void Fence::reset() {
    device().resetFences(m_fence);
}

}