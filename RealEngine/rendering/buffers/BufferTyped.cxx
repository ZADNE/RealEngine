/*!
 *  @author    Dubsky Tomas
 */
module RealEngine.rendering.buffers.BufferTyped;


namespace RE {

template<Renderer R>
void BufferTyped<R>::changeType(BufferType type) {
    m_index.type = type;
    m_index.bindingIndex = std::numeric_limits<unsigned int>::max();
}

template<Renderer R>
void BufferTyped<R>::changeType(BufferTypedIndex index) {
    m_index = index;
}

template<Renderer R>
void BufferTyped<R>::bind() {
    Buffer<R>::bind(m_index.type);
}

template<Renderer R>
void BufferTyped<R>::bindIndexed() {
    Buffer<R>::bindIndexed(m_index);
}

template BufferTyped<RendererLateBind>;
template BufferTyped<RendererGL46>;

}