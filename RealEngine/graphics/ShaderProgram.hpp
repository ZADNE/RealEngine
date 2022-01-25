#pragma once
#include <string>

#include <GL/glew.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <RealEngine/main/Error.hpp>


namespace RE {

enum class ShaderType {
	VERTEX_SHADER,
	TESS_CONTROL_SHADER,
	TESS_EVALUATION_SHADER,
	GEOMETRY_SHADER,
	FRAGMENT_SHADER,
	COMPUTE_SHADER,

	NUMBER_OF_TYPES
};

/**
* @brief POD representing source codes for all shaders within a program
*
* Nullptr means that the shader stage is unused.
*/
struct ShaderProgramSource {

	const char* const& operator[](ShaderType type) const {
		switch (type) {
		case RE::ShaderType::VERTEX_SHADER:
			return vert;
		case RE::ShaderType::TESS_CONTROL_SHADER:
			return tesc;
		case RE::ShaderType::TESS_EVALUATION_SHADER:
			return tese;
		case RE::ShaderType::GEOMETRY_SHADER:
			return geom;
		case RE::ShaderType::FRAGMENT_SHADER:
			return frag;
		case RE::ShaderType::COMPUTE_SHADER:
			return comp;
		default:
			fatalError("Accessed unknown shader type");
		}
	}

	bool operator==(const ShaderProgramSource& other) const {
		if (vert == other.vert && tesc == other.tesc && tese == other.tese
			&& geom == other.geom && frag == other.frag && comp == other.comp) {
			return true;
		}
		return false;
	}

	const char* vert = nullptr;
	const char* tesc = nullptr;
	const char* tese = nullptr;
	const char* geom = nullptr;
	const char* frag = nullptr;
	const char* comp = nullptr;

	const static ShaderProgramSource stdSprite;
	const static ShaderProgramSource stdGeometry;
};

/**
* @brief Represents a shader program used for drawing.
*/
class ShaderProgram {
	friend class UniformManager;
public:
	/**
	 * @brief Constructs shader program from given source codes
	 * @param source Source codes of the program
	*/
	ShaderProgram(const ShaderProgramSource& source);

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

	void setUniform(int location, float val) const;/**< Sets float uniform at given location */
	void setUniform(int location, glm::vec2 val) const;/**< Sets vec2 uniform at given location */
	void setUniform(int location, glm::vec3 val) const;/**< Sets vec3 uniform at given location */
	void setUniform(int location, glm::vec4 val) const;/**< Sets vec4 uniform at given location */
	void setUniform(int location, int count, const float* val) const;/**< Sets float array uniform at given location */
	void setUniform(int location, int count, const glm::vec2* val) const;/**< Sets vec2 array uniform at given location */
	void setUniform(int location, int count, const glm::vec3* val) const;/**< Sets vec3 array uniform at given location */
	void setUniform(int location, int count, const glm::vec4* val) const;/**< Sets vec4 array uniform at given location */

	void setUniform(int location, int val) const;/**< Sets int uniform at given location */
	void setUniform(int location, glm::ivec2 val) const;/**< Sets ivec2 uniform at given location */
	void setUniform(int location, glm::ivec3 val) const;/**< Sets ivec3 uniform at given location */
	void setUniform(int location, glm::ivec4 val) const;/**< Sets ivec4 uniform at given location */
	void setUniform(int location, int count, const int* val) const;/**< Sets int array uniform at given location */
	void setUniform(int location, int count, const glm::ivec2* val) const;/**< Sets ivec2 array uniform at given location */
	void setUniform(int location, int count, const glm::ivec3* val) const;/**< Sets ivec3 array uniform at given location */
	void setUniform(int location, int count, const glm::ivec4* val) const;/**< Sets ivec4 array uniform at given location */

	void setUniform(int location, unsigned int val) const;/**< Sets unsigned int uniform at given location */
	void setUniform(int location, glm::uvec2 val) const;/**< Sets uvec2 uniform at given location */
	void setUniform(int location, glm::uvec3 val) const;/**< Sets uvec3 uniform at given location */
	void setUniform(int location, glm::uvec4 val) const;/**< Sets uvec4 uniform at given location */
	void setUniform(int location, int count, const unsigned int* val) const;/**< Sets unsigned int array uniform at given location */
	void setUniform(int location, int count, const glm::uvec2* val) const;/**< Sets uvec4 array uniform at given location */
	void setUniform(int location, int count, const glm::uvec3* val) const;/**< Sets uvec4 array uniform at given location */
	void setUniform(int location, int count, const glm::uvec4* val) const;/**< Sets uvec4 array uniform at given location */

private:
	//Used to compile from scratch
	void compileProgram(const ShaderProgramSource& source);

	//Helper function
	void compileShader(const char* source, GLuint shaderID);

	//Helper function
	void linkProgram();

	//Prints program's info log
	void printProgramInfoLog() const;

	/**
	 * @brief Gets internal ID of the program
	 * @return Internal ID of the program
	*/
	GLuint getProgramID() const {
		return m_ID;
	}

	GLuint m_ID = 0;/**< OpenGL name of the program */

#ifdef _DEBUG
	static inline GLuint m_currentlyUsedID = 0u;
#endif // _DEBUG
};

}