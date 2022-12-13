/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/DescriptorSet.hpp>

#include <RealEngine/rendering/internal_renderers/VK13DescriptorSet.hpp>

namespace RE {

template<Renderer R>
DescriptorSet<R>::DescriptorSet(const Pipeline<R>& pl, vk::DescriptorType type, uint32_t binding, const Buffer<R>& bf, vk::DeviceSize offset, vk::DeviceSize range) :
    m_id(s_impl->construct(pl.m_id, type, binding, bf.m_id, offset, range)) {
}

template<Renderer R>
DescriptorSet<R>::DescriptorSet(DescriptorSet<R>&& other) noexcept :
    m_id(std::move(other.m_id)) {
}

template<Renderer R>
DescriptorSet<R>& DescriptorSet<R>::operator=(DescriptorSet<R>&& other) noexcept {
    m_id = std::move(other.m_id);
    return *this;
}

template<Renderer R>
DescriptorSet<R>::~DescriptorSet() {
    s_impl->destruct(m_id);
}

template class DescriptorSet<RendererLateBind>;
template class DescriptorSet<RendererVK13>;

}
