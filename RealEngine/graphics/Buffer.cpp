#include <RealEngine/graphics/Buffer.hpp>

namespace RE {

GLenum bufferAccesToGLEnum(BufferAccessFrequency accessFreq, BufferAccessNature accessNature) {
	static const GLenum enums[] = {
		GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY,
		GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY,
		GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, GL_DYNAMIC_COPY
	};
	return enums[static_cast<int>(accessFreq) * 3 + static_cast<int>(accessNature)];
}

}