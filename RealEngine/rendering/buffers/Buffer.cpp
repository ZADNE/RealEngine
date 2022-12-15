/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/buffers/Buffer.hpp>

#include <cassert>

#include <RealEngine/rendering/internal_renderers/VK13Buffer.hpp>


namespace RE {

template<Renderer R>
Buffer<R>::Buffer(size_t sizeInBytes, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memProperty, const void* data/* = nullptr*/) :
    m_id(s_impl->construct(sizeInBytes, usage, memProperty, data)),
    m_sizeInBytes(sizeInBytes) {
}

template<Renderer R>
Buffer<R>::Buffer(Buffer<R>&& other) noexcept :
    m_id(std::move(other.m_id)),
    m_sizeInBytes(other.m_sizeInBytes) {
}

template<Renderer R>
Buffer<R>& Buffer<R>::operator=(Buffer<R>&& other) noexcept {
    m_id = std::move(other.m_id);
    m_sizeInBytes = other.m_sizeInBytes;
    return *this;
}

template<Renderer R>
Buffer<R>::~Buffer() {
    s_impl->destruct(m_id);
}

template<Renderer R>
void Buffer<R>::unmap() const {
    s_impl->unmap(m_id);
}

template<Renderer R>
void Buffer<R>::bindAsVertexBuffer(uint32_t binding, uint64_t offsetInBytes) const {
    s_impl->bindAsVertexBuffer(m_id, binding, offsetInBytes);
}

template<Renderer R>
void Buffer<R>::bindAsIndexBuffer(uint64_t offsetInBytes, vk::IndexType indexType) const {
    s_impl->bindAsIndexBuffer(m_id, offsetInBytes, indexType);
}

template<Renderer R>
void* Buffer<R>::map(size_t offsetInBytes, size_t lengthInBytes) const {
    return s_impl->map(m_id, offsetInBytes, lengthInBytes);
}

template class Buffer<RendererLateBind>;
template class Buffer<RendererVK13>;

}