/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <limits>

#include <RealEngine/graphics/buffers/Buffer.hpp>

namespace RE {

enum class BindNow {
	NO,
	YES
};

/**
 * @brief Is buffer that holds its type (and possibly binding index) as a part of its state.
*/
class TypedBuffer : public Buffer {
public:
	using enum BufferType;
	using enum BufferStorage;
	using enum BufferAccessFrequency;
	using enum BufferAccessNature;
	using enum BufferUsageFlags;

	template<typename... Args>
	TypedBuffer(BufferType type, Args... args) :
		TypedBuffer(BufferTypedIndex{type, std::numeric_limits<GLuint>::max()}, BindNow::YES, args...) {}

	template<typename... Args>
	TypedBuffer(BufferTypedIndex index, Args... args) :
		TypedBuffer(index, BindNow::YES, args...) {}

	template<typename... Args>
	TypedBuffer(BufferTypedIndex index, BindNow bindNow, Args... args) :
		Buffer(args...),
		m_index(index) {
		if (bindNow == BindNow::YES) {
			bindIndexed();
		}
	}

	void changeType(BufferType type);

	void changeType(BufferTypedIndex index);

	BufferType getType() const { return m_index.type; }

	GLuint getBindingIndex() const { return m_index.bindingIndex; }

	using Buffer::bind;
	void bind();

	using Buffer::bindIndexed;
	void bindIndexed();

protected:
	BufferTypedIndex m_index;
};

}