/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/buffers/TypedBuffer.hpp>

#include <RealEngine/rendering/RendererLateBind.hpp>
#include <RealEngine/rendering/RendererGL46.hpp>

namespace RE {

template<typename R>
void TypedBuffer<R>::changeType(BufferType type) {
    m_index.type = type;
    m_index.bindingIndex = std::numeric_limits<unsigned int>::max();
}

template<typename R>
void TypedBuffer<R>::changeType(BufferTypedIndex index) {
    m_index = index;
}

template<typename R>
void TypedBuffer<R>::bind() {
    Buffer<R>::bind(m_index.type);
}

template<typename R>
void TypedBuffer<R>::bindIndexed() {
    Buffer<R>::bindIndexed(m_index);
}

template TypedBuffer<RendererLateBind>;
template TypedBuffer<RendererGL46>;

}