﻿/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>
#include <compare>
#include <vector>
#include <string_view>
#include <initializer_list>

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <RealEngine/main/Error.hpp>
#include <RealEngine/rendering/buffers/Buffer.hpp>
#include <RealEngine/rendering/textures/TextureUnit.hpp>
#include <RealEngine/rendering/textures/ImageUnit.hpp>

namespace RE {

/**
 * @brief Enum of all available shader stages.
*/
enum class ShaderType : GLenum {
	VERTEX = GL_VERTEX_SHADER,
	TESS_CONTROL = GL_TESS_CONTROL_SHADER,
	TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
	GEOMETRY = GL_GEOMETRY_SHADER,
	FRAGMENT = GL_FRAGMENT_SHADER,
	COMPUTE = GL_COMPUTE_SHADER
};

/**
* @brief Represents source codes of a shader stage
*/
class ShaderSources {
	friend class GL46_Renderer;
	friend class GL46_ShaderProgram;
	template<typename T>friend struct std::hash;
public:
	/**
	 * @brief This string is prepended before the first given shader source
	*/
	inline static std::string_view preamble = "#version 460 core\n";

	/**
	 * @brief Constructs empty shader source
	 *
	 * This shader stage will not be used.
	*/
	ShaderSources() {

	}

	/**
	 * @brief Constructs shader source from a single string
	 * @param source Source code of the shader
	*/
	ShaderSources(std::string_view source) :
		m_sources({preamble.data(), source.data()}),
		m_lengths({static_cast<GLint>(preamble.size()), static_cast<GLint>(source.size())}) {

	}

	/**
	 * @brief Constructs shader source from initalizer list of strings
	 * @param list List of sources
	*/
	ShaderSources(std::initializer_list<std::string_view> list) :
		m_sources(list.size() + 1u),
		m_lengths(list.size() + 1u) {
		m_sources.emplace_back(preamble.data());
		m_lengths.emplace_back(static_cast<GLint>(preamble.size()));
		for (auto& sw : list) {
			m_sources.emplace_back(sw.data());
			m_lengths.emplace_back(static_cast<GLint>(sw.size()));
		}
	}

	auto operator<=>(const ShaderSources&) const = default;

private:
	std::vector<const char*> m_sources{};/**< C-strings containing the sources */
	std::vector<GLint> m_lengths{};/**< Lengths of the sources */
};

/**
* @brief POD representing source codes for all shaders within a shader program
*/
struct ShaderProgramSources {

	const ShaderSources& operator[](ShaderType type) const {
		switch (type) {
		case RE::ShaderType::VERTEX:
			return vert;
		case RE::ShaderType::TESS_CONTROL:
			return tesc;
		case RE::ShaderType::TESS_EVALUATION:
			return tese;
		case RE::ShaderType::GEOMETRY:
			return geom;
		case RE::ShaderType::FRAGMENT:
			return frag;
		case RE::ShaderType::COMPUTE:
			return comp;
		default:
			fatalError("Tried to access invalid shader type");
		}
	}

	auto operator<=>(const ShaderProgramSources&) const = default;

	/**
	 * @brief Performs shallow equality test.
	 *
	 * Sources are considered equal when all their shaders point
	 * to same memory location. The contents of strings are NOT compared.
	*/
	bool operator==(const ShaderProgramSources& other) const {
		return vert == other.vert &&
			tesc == other.tesc &&
			tese == other.tese &&
			geom == other.geom &&
			frag == other.frag &&
			comp == other.comp;
	}

	ShaderSources vert{};/**< Vertex shader stage of the program */
	ShaderSources tesc{};/**< Tesselation control shader stage of the program */
	ShaderSources tese{};/**< Tesselation evaluation stage of the program */
	ShaderSources geom{};/**< Geometry shader stage of the program */
	ShaderSources frag{};/**< Fragment shader stage of the program */
	ShaderSources comp{};/**< Copute shader stage of the program */
};

class ShaderProgram;

/**
* @brief Is a renderer-agnostic interface to ShaderProgram's implementation.
*
* This is used internally by ShaderProgram.
*
* @see RE::ShaderProgram
*/
class IShaderProgram {
public:
	virtual void construct(ShaderProgram& sp, const ShaderProgramSources& sources) const = 0;
	virtual void destruct(ShaderProgram& sp) const = 0;


	virtual void use(const ShaderProgram& sp) const = 0;
	virtual void unuse(const ShaderProgram& sp) const = 0;

	virtual void dispatchCompute(const ShaderProgram& sp, const glm::uvec3& groupCount, bool use) const = 0;
	virtual void dispatchCompute(const ShaderProgram& sp, GLintptr indirect, bool use) const = 0;

	virtual void printInfo(const ShaderProgram& sp) const = 0;
	virtual void backInterfaceBlock(const ShaderProgram& sp, GLuint interfaceBlockIndex, const BufferTypedIndex& index) const = 0;

	virtual int getUniformLocation(const ShaderProgram& sp, const std::string& name) const = 0;

	virtual void setUniform(const ShaderProgram& sp, int location, float val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, float val0, float val1) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, const glm::vec2& val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, float val0, float val1, float val2) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, const glm::vec3& val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, float val0, float val1, float val2, float val3) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, const glm::vec4& val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const float* val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const glm::vec2* val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const glm::vec3* val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const glm::vec4* val) const = 0;

	virtual void setUniform(const ShaderProgram& sp, int location, GLint val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, GLint val0, GLint val1) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, const glm::ivec2& val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, GLint val0, GLint val1, GLint val2) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, const glm::ivec3& val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, GLint val0, GLint val1, GLint val2, GLint val3) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, const glm::ivec4& val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const GLint* val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const glm::ivec2* val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const glm::ivec3* val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const glm::ivec4* val) const = 0;

	virtual void setUniform(const ShaderProgram& sp, int location, GLuint val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, GLuint val0, GLuint val1) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, const glm::uvec2& val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, GLuint val0, GLuint val1, GLuint val2) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, const glm::uvec3& val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, GLuint val0, GLuint val1, GLuint val2, GLuint val3) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, const glm::uvec4& val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const GLuint* val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const glm::uvec2* val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const glm::uvec3* val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const glm::uvec4* val) const = 0;


	virtual void setUniform(const ShaderProgram& sp, int location, int count, const glm::mat4* val) const = 0;

	virtual void setUniform(const ShaderProgram& sp, int location, TextureUnit unit) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, ImageUnit unit) const = 0;

};

}