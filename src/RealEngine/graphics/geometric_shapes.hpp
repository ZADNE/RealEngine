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
		//Solid color
		CirclePOCO(const CirclePO& circle, RE::Color color) : CirclePO(circle), mid(color), edge(color) {}
		//Transition in colors
		CirclePOCO(const CirclePO& circle, RE::Color middle, RE::Color edgE) : CirclePO(circle), mid(middle), edge(edgE) {}

		RE::Color mid;
		RE::Color edge;
	};

}