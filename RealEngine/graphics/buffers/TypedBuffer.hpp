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
		p_index(index) {
		if (bindNow == BindNow::YES) {
			bindIndexed();
		}
	}

	void changeType(BufferType type);

	void changeType(BufferTypedIndex index);

	BufferType getType() const { return p_index.type; }

	GLuint getBindingIndex() const { return p_index.bindingIndex; }

	using Buffer::bind;
	void bind();

	using Buffer::unbind;
	void unbind();

	using Buffer::bindIndexed;
	void bindIndexed();

	using Buffer::connectToInterfaceBlock;
	void connectToInterfaceBlock(const ShaderProgram& shaderProgram, GLuint interfaceBlockIndex);

protected:
	BufferTypedIndex p_index;
};

}