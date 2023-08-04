/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/output_control/RenderPass.hpp>

namespace re {

RenderPass::RenderPass(const vk::RenderPassCreateInfo2& createInfo)
    : m_renderPass(device().createRenderPass2(createInfo)) {
}

RenderPass::RenderPass(RenderPass&& other) noexcept
    : m_renderPass(other.m_renderPass) {
    other.m_renderPass = nullptr;
}

RenderPass& RenderPass::operator=(RenderPass&& other) noexcept {
    std::swap(m_renderPass, other.m_renderPass);
    return *this;
}

RenderPass::~RenderPass() {
    deletionQueue().enqueueDeletion(m_renderPass);
}

} // namespace re
