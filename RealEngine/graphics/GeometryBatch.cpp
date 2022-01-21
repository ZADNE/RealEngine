#include <RealEngine/graphics/GeometryBatch.hpp>

#include <cmath>

#include <RealEngine/resources/ResourceManager.hpp>

#include <RealEngine/utility/utility.hpp>

namespace RE {

	GeometryBatch::GeometryBatch() {
		if (m_vao == 0) {//If doesn't exist already
			glGenVertexArrays(1, &m_vao);
		}
		glBindVertexArray(m_vao);

		if (m_vbo == 0) {//If doesn't exist already
			glGenBuffers(1, &m_vbo);
		}
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		glEnableVertexAttribArray(ATTLOC_PO);//Position
		glVertexAttribPointer(ATTLOC_PO, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPOCO), (void*)offsetof(VertexPOCO, position));
		glEnableVertexAttribArray(ATTLOC_CO);//Colour
		glVertexAttribPointer(ATTLOC_CO, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VertexPOCO), (void*)offsetof(VertexPOCO, colour));

		glBindVertexArray(0);
	}

	GeometryBatch::~GeometryBatch() {
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(1, &m_vbo);
	}

	void GeometryBatch::begin() {
		//Clearing all previous vertices
		for (auto& vector : m_vertices) {
			vector.clear();
		}
		//Clearing all previous indices
		for (auto& vector : m_indices) {
			vector.clear();
		}
	}

	void GeometryBatch::end() {
		//Uploading to VBO
		size_t totalSize = 0u;
		for (auto& vector : m_vertices) {
			totalSize += vector.size();
		}
		size_t offset = 0u;

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, totalSize * sizeof(VertexPOCO), NULL, GL_STREAM_DRAW);//Oprhaning the buffer
		for (size_t i = 0u; i < PRIMITIVES_COUNT + SHAPES_COUNT; ++i) {
			if (m_vertices[i].empty()) {
				continue;
			}
			glBufferSubData(GL_ARRAY_BUFFER, (GLintptr)offset, m_vertices[i].size() * sizeof(VertexPOCO), m_vertices[i].data());//Uploading
			offset += m_vertices[i].size() * sizeof(VertexPOCO);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void GeometryBatch::draw() {
		m_shader->setShader();
		glBindVertexArray(m_vao);

		size_t offset = 0u;

		for (size_t i = 0u; i < PRIMITIVES_COUNT + SHAPES_COUNT; ++i) {
			if (m_vertices[i].empty()) {
				continue;
			}
			glDrawElementsBaseVertex(prim_shapeToGLenum(i), (GLsizei)m_indices[i].size(), GL_UNSIGNED_INT, (void*)m_indices[i].data(), (GLint)offset);
			offset += m_vertices[i].size();
		}

		glBindVertexArray(0);
		m_shader->resetShader();
	}

	void GeometryBatch::addPrimitives(PRIM prim, size_t first, size_t countVer, const RE::VertexPOCO* data, bool separate/* = true*/) {
		GLuint last = (GLuint)m_vertices[(size_t)prim].size();
		//Vertices
		size_t prevSize = m_vertices[(size_t)prim].size();
		m_vertices[(size_t)prim].resize(prevSize + countVer);
		std::copy(&data[first], &data[first + countVer], &m_vertices[(size_t)prim][prevSize]);
		m_indices[(size_t)prim].reserve(m_indices[(size_t)prim].size() + countVer + separate);
		for (size_t i = 0u; i < countVer; ++i) {
			m_indices[(size_t)prim].push_back(last++);
		}
		//Separator
		if (separate && (prim == PRIM::LINE_STRIP || prim == PRIM::LINE_LOOP || prim == PRIM::TRIANGLE_STRIP || prim == PRIM::TRIANGLE_FAN)) {
			m_indices[(size_t)prim].push_back(PRIMITIVE_RESTART_INDEX);
		}
	}

	void GeometryBatch::addCircles(size_t first, size_t count, const RE::CirclePOCO* data) {
		GLuint last;
		VertexPOCO mid;
		VertexPOCO edge;
		for (size_t i = 0u; i < count; ++i) {//For each circle
			size_t index = (size_t)(data[first + i].disc ? SHAPE::DISC : SHAPE::CIRCLE);
			last = (GLuint)m_vertices[index].size();
			mid.position = data[first + i].pos;
			mid.colour = data[first + i].mid;
			edge.colour = data[first + i].edge;
			if (data[first + i].disc) {
				m_vertices[index].push_back(mid);
				m_indices[index].push_back(last++);
			}
			unsigned int rad = (unsigned int)data[first + i].rad;
			rad--; rad |= rad >> 1; rad |= rad >> 2; rad |= rad >> 4; rad |= rad >> 8; rad |= rad >> 16; rad++;
			rad = (unsigned int)std::log2<unsigned int>(rad);
			rad = (rad + 4u) / 2u;
			float steps = std::pow(2.0f, (float)rad);
			for (float j = 0.0f; j < steps; ++j) {//For each edge
				float d = j * rmath::PI_f * 2.0f / steps;
				edge.position.x = mid.position.x + cos(d) * data[first + i].rad;
				edge.position.y = mid.position.y + sin(d) * data[first + i].rad;
				m_vertices[index].push_back(edge);
				m_indices[index].push_back(last++);
			}
			if (data[first + i].disc) {
				m_indices[index].push_back(last - (int)steps);
			}
			m_indices[index].push_back(PRIMITIVE_RESTART_INDEX);
		}
	}

	void GeometryBatch::changeShader(ShaderProgramSource source/* = ShaderProgramSource::stdGeometry*/) {
		m_shader = RE::RM::getShaderProgram(source);
	}

	GLenum GeometryBatch::prim_shapeToGLenum(size_t prim_shape) const {
		switch (prim_shape) {
		case (size_t)PRIM::POINTS:
			return GLenum(GL_POINTS);
		case (size_t)PRIM::LINE_STRIP:
			return GLenum(GL_LINE_STRIP);
		case (size_t)PRIM::LINE_LOOP:
			return GLenum(GL_LINE_LOOP);
		case (size_t)PRIM::LINES:
			return GLenum(GL_LINES);
		case (size_t)PRIM::TRIANGLE_STRIP:
			return GLenum(GL_TRIANGLE_STRIP);
		case (size_t)PRIM::TRIANGLE_FAN:
			return GLenum(GL_TRIANGLE_FAN);
		case (size_t)PRIM::TRIANGLES:
			return GLenum(GL_TRIANGLES);
		case (size_t)SHAPE::CIRCLE:
			return GLenum(GL_LINE_LOOP);
		case (size_t)SHAPE::DISC:
			return GLenum(GL_TRIANGLE_FAN);
		default:
			//Shouldn't get here
			return GLenum(GL_LINE_LOOP);
		}
	}

}