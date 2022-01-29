#pragma once
#include <string>
#include <compare>

#include <GL/glew.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <RealEngine/main/Error.hpp>
#include <RealEngine/graphics/texture/TextureUnit.hpp>


namespace RE {

enum class ShaderType : GLenum {
	VERTEX = GL_VERTEX_SHADER,
	TESS_CONTROL = GL_TESS_CONTROL_SHADER,
	TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
	GEOMETRY = GL_GEOMETRY_SHADER,
	FRAGMENT = GL_FRAGMENT_SHADER,
	COMPUTE = GL_COMPUTE_SHADER
};

/**
* @brief POD representing source codes for all shaders within a program
*
* Nullptr means that the shader stage is unused.
*/
struct ShaderProgramSource {

	const char* const& operator[](ShaderType type) const {
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

	auto operator<=>(const ShaderProgramSource&) const = default;

	const char* vert = nullptr;/**< Vertex shader stage of the program */
	const char* tesc = nullptr;/**< Tesselation control shader stage of the program */
	const char* tese = nullptr;/**< Tesselation evaluation stage of the program */
	const char* geom = nullptr;/**< Geometry shader stage of the program */
	const char* frag = nullptr;/**< Fragment shader stage of the program */
	const char* comp = nullptr;/**< Copute shader stage of the program */
};

/**
* @brief Controls how vertices are rendered to screen.
*/
class ShaderProgram {
	friend class UniformBuffer;
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
	void compileProgram(const ShaderProgramSource& source);

	//Helper function
	void compileShader(const char* source, GLuint shaderID);

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