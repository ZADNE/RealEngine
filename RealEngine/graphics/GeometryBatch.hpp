﻿#pragma once
#include <array>
#include <vector>

#include <GL/glew.h>

#include <RealEngine/graphics/Vertex.hpp>
#include <RealEngine/resources/ResourceManager.hpp>
#include <RealEngine/graphics/default_shaders.hpp>
#include <RealEngine/graphics/geometric_shapes.hpp>

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

	class GeometryBatch {
	public:
		GeometryBatch();
		~GeometryBatch();

		void begin();
		void end();

		void draw();

		void addPrimitives(PRIM prim, size_t first, size_t count, const RE::VertexPOCO* data, bool separate = true);

		void addCircles(size_t first, size_t count, const RE::CirclePOCO* data);

		void changeShader(ShaderProgramSource source = ShaderProgramSource::stdGeometry);

		static GeometryBatch& std() {
			static GeometryBatch std{};
			return std;
		}

		GLenum prim_shapeToGLenum(size_t prim_shape) const;
	private:
		GLuint m_vbo = 0;
		GLuint m_vao = 0;

		std::array<std::vector<RE::VertexPOCO>, PRIMITIVES_COUNT + SHAPES_COUNT> m_vertices;
		std::array<std::vector<GLuint>, PRIMITIVES_COUNT + SHAPES_COUNT> m_indices;

		ShaderProgramPtr m_shader = RE::RM::getShaderProgram(ShaderProgramSource::stdGeometry);
	};

}