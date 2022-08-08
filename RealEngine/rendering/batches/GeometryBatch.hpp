/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <array>
#include <vector>

#include <GL/glew.h>

#include <RealEngine/rendering/vertices/vertices.hpp>
#include <RealEngine/rendering/batches/circles.hpp>
#include <RealEngine/rendering/vertices/VertexArray.hpp>
#include <RealEngine/rendering/buffers/Buffer.hpp>
#include <RealEngine/resources/ShaderProgramCache.hpp>

namespace RE {

enum class PRIM {
	POINTS,
	LINE_STRIP,
	LINE_LOOP,
	LINES,
	TRIANGLE_STRIP,
	TRIANGLE_FAN,
	TRIANGLES
};

enum class SHAPE {
	CIRCLE = (int)PRIM::TRIANGLES + 1,
	DISC
};

const size_t PRIMITIVES_COUNT = 7u;

const size_t SHAPES_COUNT = 2u;

/**
 * @brief Draws lines, circles and other vector shapes.
*/
class GeometryBatch {
public:
	GeometryBatch();

	void begin();
	void end();

	void draw();

	void addPrimitives(PRIM prim, size_t first, size_t count, const RE::VertexPOCO* data, bool separate = true);

	void addCircles(size_t first, size_t count, const RE::CirclePOCO* data);

	void switchShaderProgram(ShaderProgramPtr shaderProgram);

	static GeometryBatch& std() {
		static GeometryBatch std{};
		return std;
	}

private:
	VertexArray m_va;
	Buffer m_buf{BufferAccessFrequency::STREAM, BufferAccessNature::DRAW};
	ShaderProgramPtr m_shaderProgram;

	std::array<std::vector<RE::VertexPOCO>, PRIMITIVES_COUNT + SHAPES_COUNT> m_vertices;
	std::array<std::vector<GLuint>, PRIMITIVES_COUNT + SHAPES_COUNT> m_indices;
};

}