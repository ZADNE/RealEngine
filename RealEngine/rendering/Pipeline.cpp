/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/Pipeline.hpp>

#include <RealEngine/rendering/internal_renderers/VK13Pipeline.hpp>

namespace RE {

template<Renderer R>
Pipeline<R>::Pipeline(const vk::PipelineVertexInputStateCreateInfo& vi, const ShaderProgramSources& srcs) :
    m_id(s_impl->construct(vi, srcs)) {
}

template<Renderer R>
Pipeline<R>::Pipeline(Pipeline<R>&& other) noexcept :
    m_id(std::move(other.m_id)) {
}

template<Renderer R>
Pipeline<R>& Pipeline<R>::operator=(Pipeline<R>&& other) noexcept {
    m_id = std::move(other.m_id);
    return *this;
}

template<Renderer R>
Pipeline<R>::~Pipeline() {
    s_impl->destruct(m_id);
}

template<Renderer R>
void Pipeline<R>::bind(vk::PipelineBindPoint bindPoint) const {
    s_impl->bind(m_id, bindPoint);
}

template<Renderer R>
void Pipeline<R>::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const {
    s_impl->draw(m_id, vertexCount, instanceCount, firstVertex, firstInstance);
}

template class Pipeline<RendererLateBind>;
template class Pipeline<RendererVK13>;

}
