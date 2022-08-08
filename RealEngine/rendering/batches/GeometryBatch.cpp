/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/batches/GeometryBatch.hpp>

#include <cmath>

#include <glm/gtc/constants.hpp>

#include <RealEngine/resources/ResourceManager.hpp>
#include <RealEngine/rendering/internal_interfaces/IVertexArray.hpp>

namespace RE {

Primitive convertPrimEnum(size_t prim_shape) {
	switch (prim_shape) {
	case (size_t)PRIM::POINTS:
		return Primitive::POINTS;
	case (size_t)PRIM::LINE_STRIP:
		return Primitive::LINE_STRIP;
	case (size_t)PRIM::LINE_LOOP:
		return Primitive::LINE_LOOP;
	case (size_t)PRIM::LINES:
		return Primitive::LINES;
	case (size_t)PRIM::TRIANGLE_STRIP:
		return Primitive::TRIANGLE_STRIP;
	case (size_t)PRIM::TRIANGLE_FAN:
		return Primitive::TRIANGLE_FAN;
	case (size_t)PRIM::TRIANGLES:
		return Primitive::TRIANGLES;
	case (size_t)SHAPE::CIRCLE:
		return Primitive::LINE_LOOP;
	case (size_t)SHAPE::DISC:
		return Primitive::TRIANGLE_FAN;
	default:
		//Shouldn't get here
		return Primitive::LINE_LOOP;
	}
}

GeometryBatch::GeometryBatch() {
	m_va.setAttribute(ATTR_POSITION, VertexComponentCount::XY, VertexComponentType::FLOAT, offsetof(VertexPOCO, position), false);
	m_va.setAttribute(ATTR_COLOR, VertexComponentCount::RGBA, VertexComponentType::UNSIGNED_BYTE, offsetof(VertexPOCO, color), true);
	m_va.setBindingPoint(0u, m_buf, 0, sizeof(VertexPOCO));
	m_va.setBindingPoint(1u, m_buf, 0, sizeof(VertexPOCO));
	m_va.connectAttributeToBindingPoint(ATTR_POSITION, 0u);
	m_va.connectAttributeToBindingPoint(ATTR_COLOR, 1u);
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

	m_buf.redefine(totalSize * sizeof(VertexPOCO), nullptr);
	for (size_t i = 0u; i < PRIMITIVES_COUNT + SHAPES_COUNT; ++i) {
		if (m_vertices[i].empty()) continue;
		m_buf.overwrite(offset, m_vertices[i]);
		offset += m_vertices[i].size() * sizeof(VertexPOCO);
	}
}

void GeometryBatch::draw() {
	m_shaderProgram->use();
	m_va.bind();

	size_t offset = 0u;

	for (size_t i = 0u; i < PRIMITIVES_COUNT + SHAPES_COUNT; ++i) {
		if (m_vertices[i].empty()) continue;
		m_va.renderElementsBaseVertex(convertPrimEnum(i), (GLsizei)m_indices[i].size(), IndexType::UNSIGNED_INT, m_indices[i].data(), (GLint)offset);
		offset += m_vertices[i].size();
	}

	m_va.unbind();
	m_shaderProgram->unuse();
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
		m_indices[(size_t)prim].push_back(PRIMITIVE_RESTART_INDEX<GLuint>());
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
		mid.color = data[first + i].mid;
		edge.color = data[first + i].edge;
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
			float d = j * glm::pi<float>() * 2.0f / steps;
			edge.position.x = mid.position.x + cos(d) * data[first + i].rad;
			edge.position.y = mid.position.y + sin(d) * data[first + i].rad;
			m_vertices[index].push_back(edge);
			m_indices[index].push_back(last++);
		}
		if (data[first + i].disc) {
			m_indices[index].push_back(last - (int)steps);
		}
		m_indices[index].push_back(PRIMITIVE_RESTART_INDEX<GLuint>());
	}
}

void GeometryBatch::switchShaderProgram(ShaderProgramPtr shaderProgram) {
	m_shaderProgram = shaderProgram;
}

}