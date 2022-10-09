/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/buffers/Buffer.hpp>

#include <cassert>

#include <RealEngine/rendering/internal_renderers/GL46Buffer.hpp>
#include <RealEngine/rendering/internal_renderers/VK13Buffer.hpp>


namespace RE {

using enum BufferType;
using enum BufferStorage;
using enum BufferAccessFrequency;
using enum BufferAccessNature;
using enum BufferUsageFlags;

template<Renderer R>
Buffer<R>::Buffer(size_t sizeInBytes, BufferUsageFlags flags, const void* data/* = nullptr*/) :
    m_id(s_impl->constructImmutable(sizeInBytes, flags, data)),
    m_sizeInBytes(sizeInBytes)
#ifndef NDEBUG
    , m_storage(IMMUTABLE) {
#else
    {
#endif // DEBUG
}

template<Renderer R>
Buffer<R>::Buffer(size_t sizeInBytes, BufferAccessFrequency accessFreq, BufferAccessNature accessNature, const void* data/* = nullptr*/) :
    m_id(s_impl->constructMutable(sizeInBytes, accessFreq, accessNature, data)),
    m_sizeInBytes(sizeInBytes)
#ifndef NDEBUG
    , m_storage(MUTABLE) {
#else
    {
#endif // DEBUG
}

template<Renderer R>
Buffer<R>::Buffer(Buffer<R>&& other) noexcept :
    m_id(std::move(other.m_id)),
    m_sizeInBytes(other.m_sizeInBytes)
#ifndef NDEBUG
    , m_storage(other.m_storage) {
#else
    {
#endif // DEBUG
}

template<Renderer R>
Buffer<R>& Buffer<R>::operator=(Buffer<R>&& other) noexcept {
    m_id = std::move(other.m_id);
    m_sizeInBytes = other.m_sizeInBytes;
#ifndef NDEBUG
    m_storage = other.m_storage;
#endif // DEBUG
    return *this;
}

template<Renderer R>
Buffer<R>::~Buffer() {
    s_impl->destruct(m_id);
}

template<Renderer R>
void Buffer<R>::bind(BufferType bindType) const {
    s_impl->bind(m_id, bindType);
}

template<Renderer R>
void Buffer<R>::bindIndexed(const BufferTypedIndex& index) const {
    s_impl->bindIndexed(m_id, index);
}

template<Renderer R>
void Buffer<R>::overwrite(size_t offsetInBytes, size_t countBytes, const void* data) const {
    s_impl->overwrite(m_id, offsetInBytes, countBytes, data);
}

template<Renderer R>
void Buffer<R>::redefine(size_t sizeInBytes, const void* data) {
    assert(m_storage == MUTABLE);
    if (sizeInBytes > m_sizeInBytes) {
        m_sizeInBytes = sizeInBytes;
        s_impl->redefine(m_id, sizeInBytes, data);
    } else {
        s_impl->overwrite(m_id, 0, sizeInBytes, data);
    }
}

template<Renderer R>
void Buffer<R>::invalidate() const {
    s_impl->invalidate(m_id);
}

template<Renderer R>
void Buffer<R>::invalidate(size_t lengthInBytes) const {
    s_impl->invalidate(m_id, lengthInBytes);
}

template<Renderer R>
void Buffer<R>::flushMapped(size_t offsetInBytes, size_t lengthInBytes) const {
    s_impl->flushMapped(m_id, offsetInBytes, lengthInBytes);
}

template<Renderer R>
bool Buffer<R>::unmap() const {
    return s_impl->unmap(m_id);
}

template<Renderer R>
size_t Buffer<R>::size() const {
    return m_sizeInBytes;
}

template<Renderer R>
void* Buffer<R>::map(size_t offsetInBytes, size_t lengthInBytes, BufferMapUsageFlags mappingUsage) const {
    return s_impl->map(m_id, offsetInBytes, lengthInBytes, mappingUsage);
}

template class Buffer<RendererLateBind>;
template class Buffer<RendererVK13>;
template class Buffer<RendererGL46>;

}