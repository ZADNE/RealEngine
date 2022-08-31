/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <limits>

#include <RealEngine/rendering/buffers/Buffer.hpp>

namespace RE {

enum class BindNow {
    NO,
    YES
};

/**
 * @brief Is buffer that holds its type (and possibly binding index) as a part of its state.
 * @tparam R The renderer that will perform the commands
*/
template<typename R = RendererLateBind>
class TypedBuffer : public Buffer<R> {
public:

    template<typename... Args>
    TypedBuffer(BufferType type, Args... args) :
        TypedBuffer(BufferTypedIndex{type, std::numeric_limits<unsigned int>::max()}, BindNow::YES, args...) {}

    template<typename... Args>
    TypedBuffer(BufferTypedIndex index, Args... args) :
        TypedBuffer(index, BindNow::YES, args...) {}

    template<typename... Args>
    TypedBuffer(BufferTypedIndex index, BindNow bindNow, Args... args) :
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