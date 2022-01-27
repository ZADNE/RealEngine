#pragma once
#include <limits>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace RE {

using Colour = glm::vec<4, uint8_t, glm::qualifier::defaultp>;

const unsigned int ATTR_POSITION = 0u;
const unsigned int ATTR_COLOUR = 1u;
const unsigned int ATTR_UV = 2u;

template<typename T>
consteval T PRIMITIVE_RESTART_INDEX() {
	static_assert(std::is_unsigned<T>::value, "primitive restart index in defined only for unsigned types");
	return std::numeric_limits<T>::max();
};

#pragma warning(push)
#pragma warning(disable: 26495)

struct VertexPO {
	VertexPO() {}
	VertexPO(const glm::vec2& pos) : position(pos) {}

	glm::vec2 position;
};

struct VertexPOCO {
	VertexPOCO() {}
	VertexPOCO(const glm::vec2& pos, Colour col) : position(pos), colour(col) {}

	glm::vec2 position;
	Colour colour;
};


struct VertexPOCOUV {
	VertexPOCOUV() {}
	VertexPOCOUV(const glm::vec2& pos, Colour col, const glm::vec2& uv) : position(pos), colour(col), uv(uv) {}

	glm::vec2 position;
	Colour colour;
	glm::vec2 uv;
};
#pragma warning(pop)

}
