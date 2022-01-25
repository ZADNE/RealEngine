#pragma once
#include <glm/vec2.hpp>

#include <RealEngine/graphics/Vertex.hpp>

namespace RE {

struct CirclePO {
	CirclePO() {}
	CirclePO(const glm::vec2& position, float radius, bool disC) : pos(position), rad(radius), disc(disC) {}

	glm::vec2 pos;
	float rad;
	bool disc;
};

struct CirclePOCO : public CirclePO {
	CirclePOCO() {}
	//Solid colour
	CirclePOCO(const CirclePO& circle, RE::Colour colour) : CirclePO(circle), mid(colour), edge(colour) {}
	//Transition in colours
	CirclePOCO(const CirclePO& circle, RE::Colour middle, RE::Colour edgE) : CirclePO(circle), mid(middle), edge(edgE) {}

	RE::Colour mid;
	RE::Colour edge;
};

}