/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/output_control/RenderPass.hpp>

namespace re {

RenderPass::RenderPass(const RenderPassCreateInfo& createInfo)
    : m_renderPass(device().createRenderPass2(vk::RenderPassCreateInfo2{
          {}, createInfo.attachments, createInfo.subpasses, createInfo.dependencies
      })) {

    setDebugUtilsObjectName(m_renderPass, createInfo.debugName);
}

RenderPass::RenderPass(RenderPass&& other) noexcept
    : m_renderPass(std::exchange(other.m_renderPass, nullptr)) {
}

RenderPass& RenderPass::operator=(RenderPass&& other) noexcept {
    std::swap(m_renderPass, other.m_renderPass);
    return *this;
}

RenderPass::~RenderPass() {
    deletionQueue().enqueueDeletion(m_renderPass);
}

} // namespace re
