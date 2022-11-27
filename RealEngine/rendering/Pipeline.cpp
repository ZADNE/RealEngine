/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/Pipeline.hpp>

#include <RealEngine/rendering/internal_renderers/GL46Pipeline.hpp>
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

template class Pipeline<RendererLateBind>;
template class Pipeline<RendererVK13>;
template class Pipeline<RendererGL46>;

}
