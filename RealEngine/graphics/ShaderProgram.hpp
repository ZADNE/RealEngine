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

#include <RealEngine/main/Error.hpp>
#include <RealEngine/graphics/texture/TextureUnit.hpp>


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
	friend class ShaderProgram;
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
		for (auto& sw: list) {
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

/**
* @brief Controls how vertices are rendered to screen.
*/
class ShaderProgram {
	friend class UniformBuffer;
public:
	/**
	 * @brief Constructs shader program from given source codes
	 * @param sources Source codes of the program
	*/
	ShaderProgram(const ShaderProgramSources& sources);

	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram(ShaderProgram&& other) noexcept;

	ShaderProgram& operator=(const ShaderProgram&) = delete;
	ShaderProgram& operator=(ShaderProgram&& other) noexcept;

	/**
	 * @brief Destructs shader program
	*/
	~ShaderProgram();

	/**
	* @brief Sets the shader active for rendering
	*/
	void use() const;

	/**
	* @brief Unsets the shader for rendering
	*/
	void unuse() const;

	/**
	* @brief Prints diagnostic information about the shader program
	*/
	void printInfo() const;


	/**
	 * @brief Resolves location of uniform "name" and sets its value
	 * @param name Name of the uniform
	*/
	template<typename... Args>
	void setUniform(const std::string& name, Args... args) const {
		auto loc = glGetUniformLocation(m_ID, name.c_str());
	#ifdef _DEBUG
		if (loc < 0) error(std::string{"Uniform \""} + name + "\" does not exist in program " + std::to_string(m_ID) + "!");
	#endif // _DEBUG
		setUniform(loc, args...);
	}

	void setUniform(int location, float val) const;										/**< Sets float uniform at given location */
	void setUniform(int location, float val0, float val1) const;						/**< Sets vec2 uniform at given location */
	void setUniform(int location, const glm::vec2& val) const;							/**< Sets vec2 uniform at given location */
	void setUniform(int location, float val0, float val1, float val2) const;			/**< Sets vec3 uniform at given location */
	void setUniform(int location, const glm::vec3& val) const;							/**< Sets vec3 uniform at given location */
	void setUniform(int location, float val0, float val1, float val2, float val3) const;/**< Sets vec4 uniform at given location */
	void setUniform(int location, const glm::vec4& val) const;							/**< Sets vec4 uniform at given location */
	void setUniform(int location, int count, const float* val) const;					/**< Sets float array uniform at given location */
	void setUniform(int location, int count, const glm::vec2* val) const;				/**< Sets vec2 array uniform at given location */
	void setUniform(int location, int count, const glm::vec3* val) const;				/**< Sets vec3 array uniform at given location */
	void setUniform(int location, int count, const glm::vec4* val) const;				/**< Sets vec4 array uniform at given location */

	void setUniform(int location, GLint val) const;										/**< Sets int uniform at given location */
	void setUniform(int location, GLint val0, GLint val1) const;						/**< Sets ivec2 uniform at given location */
	void setUniform(int location, const glm::ivec2& val) const;							/**< Sets ivec2 uniform at given location */
	void setUniform(int location, GLint val0, GLint val1, GLint val2) const;			/**< Sets ivec3 uniform at given location */
	void setUniform(int location, const glm::ivec3& val) const;							/**< Sets ivec3 uniform at given location */
	void setUniform(int location, GLint val0, GLint val1, GLint val2, GLint val3) const;/**< Sets ivec4 uniform at given location */
	void setUniform(int location, const glm::ivec4& val) const;							/**< Sets ivec4 uniform at given location */
	void setUniform(int location, int count, const GLint* val) const;					/**< Sets int array uniform at given location */
	void setUniform(int location, int count, const glm::ivec2* val) const;				/**< Sets ivec2 array uniform at given location */
	void setUniform(int location, int count, const glm::ivec3* val) const;				/**< Sets ivec3 array uniform at given location */
	void setUniform(int location, int count, const glm::ivec4* val) const;				/**< Sets ivec4 array uniform at given location */

	void setUniform(int location, GLuint val) const;									/**< Sets unsigned int uniform at given location */
	void setUniform(int location, GLuint val0, GLuint val1) const;						/**< Sets uvec2 uniform at given location */
	void setUniform(int location, const glm::uvec2& val) const;							/**< Sets uvec2 uniform at given location */
	void setUniform(int location, GLuint val0, GLuint val1, GLuint val2) const;			/**< Sets uvec3 uniform at given location */
	void setUniform(int location, const glm::uvec3& val) const;							/**< Sets uvec3 uniform at given location */
	void setUniform(int location, GLuint val0, GLuint val1, GLuint val2, GLuint val3) const;/**< Sets uvec4 uniform at given location */
	void setUniform(int location, const glm::uvec4& val) const;							/**< Sets uvec4 uniform at given location */
	void setUniform(int location, int count, const GLuint* val) const;					/**< Sets unsigned int array uniform at given location */
	void setUniform(int location, int count, const glm::uvec2* val) const;				/**< Sets uvec4 array uniform at given location */
	void setUniform(int location, int count, const glm::uvec3* val) const;				/**< Sets uvec4 array uniform at given location */
	void setUniform(int location, int count, const glm::uvec4* val) const;				/**< Sets uvec4 array uniform at given location */

	void setUniform(int location, TextureUnit texUnit) const;							/**< Sets sampler uniform at given location */

private:
	//Used to compile from scratch
	void compileProgram(const ShaderProgramSources& source);

	//Helper function
	void compileShader(const ShaderSources& source, GLuint shaderID);

	//Helper function
	void linkProgram();

	//Prints program's info log
	void printProgramInfoLog() const;

	/**
	 * @brief Gets internal ID of the program
	 *
	 * Use with caution!
	 *
	 * @return Internal ID of the program
	*/
	/*GLuint getProgramID() const {
		return m_ID;
	}*/

	GLuint m_ID = 0;/**< OpenGL name of the program */

#ifdef _DEBUG
	static inline GLuint m_currentlyUsedID = 0u;
#endif // _DEBUG
};

}