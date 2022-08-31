/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/buffers/Buffer.hpp>

#include <RealEngine/rendering/internal_interfaces/IBuffer.hpp>

#include <RealEngine/rendering/RendererLateBind.hpp>
#include <RealEngine/rendering/RendererGL46.hpp>

namespace RE {

using enum BufferType;
using enum BufferStorage;
using enum BufferAccessFrequency;
using enum BufferAccessNature;
using enum BufferUsageFlags;

template <typename R>
Buffer<R>::Buffer(size_t sizeInBytes, BufferUsageFlags flags, const void* data/* = nullptr*/) :
    m_internals(s_impl->constructImmutable(sizeInBytes, flags, data)) {
}

template <typename R>
Buffer<R>::Buffer(size_t sizeInBytes, BufferAccessFrequency accessFreq, BufferAccessNature accessNature, const void* data/* = nullptr*/) :
    m_internals(s_impl->constructMutable(sizeInBytes, accessFreq, accessNature, data)) {
}

template <typename R>
Buffer<R>::~Buffer() {
    s_impl->destruct(m_internals);
}

template <typename R>
Buffer<R>::Buffer(Buffer<R>&& other) noexcept :
    m_internals(std::move(other.m_internals)) {
}

template <typename R>
Buffer<R>& Buffer<R>::operator=(Buffer<R>&& other) noexcept {
    m_internals = std::move(other.m_internals);
    return *this;
}

template <typename R>
void Buffer<R>::bind(BufferType bindType) const {
    s_impl->bind(m_internals, bindType);
}

template <typename R>
void Buffer<R>::bindIndexed(const BufferTypedIndex& index) const {
    s_impl->bindIndexed(m_internals, index);
}

template <typename R>
void Buffer<R>::overwrite(size_t offsetInBytes, size_t countBytes, const void* data) const {
    s_impl->overwrite(m_internals, offsetInBytes, countBytes, data);
}

template <typename R>
void Buffer<R>::redefine(size_t sizeInBytes, const void* data) {
    s_impl->redefine(m_internals, sizeInBytes, data);
}

template <typename R>
void Buffer<R>::invalidate() const {
    s_impl->invalidate(m_internals);
}

template <typename R>
void Buffer<R>::invalidate(size_t lengthInBytes) const {
    s_impl->invalidate(m_internals, lengthInBytes);
}

template <typename R>
void Buffer<R>::flushMapped(size_t offsetInBytes, size_t lengthInBytes) const {
    s_impl->flushMapped(m_internals, offsetInBytes, lengthInBytes);
}

template <typename R>
bool Buffer<R>::unmap() const {
    return s_impl->unmap(m_internals);
}

template <typename R>
size_t Buffer<R>::size() const {
    return m_internals.size();
}

template Buffer<RendererLateBind>;
template Buffer<RendererGL46>;

}