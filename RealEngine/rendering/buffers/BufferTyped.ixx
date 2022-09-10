/*!
 *  @author    Dubsky Tomas
 */
export module RealEngine.rendering.buffers.BufferTyped;
import std;
export import RealEngine.rendering.buffers.Buffer;
export import RealEngine.rendering.Renderer;


export namespace RE {

enum class BindNow {
    NO,
    YES
};

/**
 * @brief Is a buffer that holds its type (and possibly binding index) as a part of its state
 * @tparam R The renderer that will perform the commands
*/
template<Renderer R = RendererLateBind>
class BufferTyped : public Buffer<R> {
public:

    template<typename... Args>
    BufferTyped(BufferType type, Args... args) :
        BufferTyped(BufferTypedIndex{type, std::numeric_limits<unsigned int>::max()}, BindNow::YES, args...) {}

    template<typename... Args>
    BufferTyped(BufferTypedIndex index, Args... args) :
        BufferTyped(index, BindNow::YES, args...) {}

    template<typename... Args>
    BufferTyped(BufferTypedIndex index, BindNow bindNow, Args... args) :
        Buffer<R>(args...),
        m_index(index) {
        if (bindNow == BindNow::YES) {
            if (isIndexedBufferType(m_index.type)) {
                bindIndexed();
            } else {
                bind();
            }
        }
    }

    void changeType(BufferType type);

    void changeType(BufferTypedIndex index);

    BufferType getType() const { return m_index.type; }

    unsigned int getBindingIndex() const { return m_index.bindingIndex; }

    using Buffer<R>::bind;
    void bind();

    using Buffer<R>::bindIndexed;
    void bindIndexed();

protected:

    BufferTypedIndex m_index;
};

}