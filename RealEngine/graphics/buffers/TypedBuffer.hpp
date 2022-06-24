/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <limits>

#include <RealEngine/graphics/buffers/Buffer.hpp>

namespace RE {

struct BufferTypedIndex {

	BufferTypedIndex(BufferType type, GLuint bindingIndex) :
		type(type), bindingIndex(bindingIndex) {
		using enum BufferType;
		assert(type == ATOMIC_COUNTER ||
			type == TRANSFORM_FEEDBACK ||
			type == UNIFORM ||
			type == SHADER_STORAGE);
	}

	BufferType type;
	GLuint bindingIndex;
};

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
	TypedBuffer(BufferTypedIndex typedIndex, Args... args) :
		TypedBuffer(typedIndex, BindNow::YES, args...) {}

	template<typename... Args>
	TypedBuffer(BufferTypedIndex typedIndex, BindNow bindNow, Args... args) :
		Buffer(args...),
		p_type(typedIndex.type),
		p_bindingIndex(typedIndex.bindingIndex) {
		if (bindNow == BindNow::YES) {
			bindIndexed();
		}
	}

	void changeType(BufferType type);

	void changeType(BufferTypedIndex typedIndex);

	BufferType getType() const { return p_type; }

	GLuint getBindingIndex() const { return p_bindingIndex; }

	using Buffer::bind;
	void bind();

	using Buffer::unbind;
	void unbind();

	using Buffer::bindIndexed;
	void bindIndexed();

	using Buffer::connectToInterfaceBlock;
	void connectToInterfaceBlock(const ShaderProgram& shaderProgram, GLuint interfaceBlockIndex);

protected:
	BufferType p_type;
	GLuint p_bindingIndex;
};

}