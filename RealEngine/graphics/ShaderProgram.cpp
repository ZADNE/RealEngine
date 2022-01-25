#include <RealEngine/graphics/ShaderProgram.hpp>

#include <fstream>
#include <vector>
#include <iostream>
#include <array>

#include <glm/gtc/type_ptr.hpp>

#include <RealEngine/utility/utility.hpp>
#include <RealEngine/main/Error.hpp>
#include <RealEngine/graphics/default_shaders.hpp>

const std::array STAGES = {
	GL_VERTEX_SHADER,
	GL_TESS_CONTROL_SHADER,
	GL_TESS_EVALUATION_SHADER,
	GL_GEOMETRY_SHADER,
	GL_FRAGMENT_SHADER,
	GL_COMPUTE_SHADER
};


namespace RE {

const ShaderProgramSource ShaderProgramSource::stdSprite{.vert = vert_sprite, .frag = frag_sprite};
const ShaderProgramSource ShaderProgramSource::stdGeometry{.vert = vert_geometry, .frag = frag_geometry};

ShaderProgram::ShaderProgram(const ShaderProgramSource& source) {
	compileProgram(source);
#if defined(_DEBUG) && defined(RE_LOG_SHADER_PROGRAM_CREATED)
	std::cout << "Created shader (ID: " << m_programID << ")\n";
#endif // defined(_DEBUG) && defined(RE_LOG_SHADER_PROGRAM_CREATED)
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept :
	m_ID(other.m_ID) {
	other.m_ID = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
	std::swap(m_ID, other.m_ID);
	return *this;
}

ShaderProgram::~ShaderProgram() {
	if (m_ID != 0) {
	#if defined(_DEBUG) && defined(RE_LOG_SHADER_PROGRAM_DESTROYED)
		std::cout << "Destroyed shader (ID: " << m_programID << ")\n";
	#endif // defined(_DEBUG) && defined(RE_LOG_SHADER_PROGRAM_DESTROYED)
		glDeleteProgram(m_ID);
	}
}

void ShaderProgram::use() const {
#ifdef _DEBUG
	if (m_currentlyUsedID != 0) {
		throw "Overbound shader programs";
	}
	m_currentlyUsedID = m_ID;
#endif // _DEBUG
	glUseProgram(m_ID);
}

void ShaderProgram::unuse() const {
#ifdef _DEBUG
	if (m_currentlyUsedID != m_ID) {
		throw "Overbound shader programs";
	}
	m_currentlyUsedID = 0;
	glBindVertexArray(0);
#endif // _DEBUG
}

void ShaderProgram::printInfo() const {
	std::cout << "|SHADER: " << m_ID << '\n';
	//ATTRIBUTE INFO
	GLint numberAttributes;
	glGetProgramiv(m_ID, GL_ACTIVE_ATTRIBUTES, &numberAttributes);
	std::cout << "|ATTRIBUTES: " << std::to_string(numberAttributes) << "\n";
	for (GLint i = 0; i < numberAttributes; i++) {
		GLsizei length;
		GLint size;
		GLenum type;
		GLchar name[32];
		glGetActiveAttrib(m_ID, i, 32, &length, &size, &type, &name[0]);
		GLint loc = glGetAttribLocation(m_ID, &name[0]);
		std::cout << "|  layout (location = " << std::to_string(loc) << ") in " << GLSLTypeToString(type) << " " << name << ((size > 1) ? ("[" + std::to_string(size) + "]") : "") << '\n';
	}

	//UNIFORM INFO
	GLint numberUniforms;
	glGetProgramiv(m_ID, GL_ACTIVE_UNIFORMS, &numberUniforms);
	std::cout << "|UNIFORMS: " << std::to_string(numberUniforms) << "\n";
	for (GLint i = 0; i < numberUniforms; i++) {
		GLsizei length;
		GLint size;
		GLenum type;
		GLchar name[32];
		glGetActiveUniform(m_ID, i, 32, &length, &size, &type, &name[0]);
		GLint loc = glGetUniformLocation(m_ID, &name[0]);
		std::cout << "|  layout (location = " << std::to_string(loc) << ") uniform " << GLSLTypeToString(type) << " " << name << ((size > 1) ? ("[" + std::to_string(size) + "]") : "") << '\n';
	}
}

void ShaderProgram::setUniform(int location, float val) const { glProgramUniform1f(m_ID, location, val); }
void ShaderProgram::setUniform(int location, glm::vec2 val) const { glProgramUniform2f(m_ID, location, val.x, val.y); }
void ShaderProgram::setUniform(int location, glm::vec3 val) const { glProgramUniform3f(m_ID, location, val.x, val.y, val.z); }
void ShaderProgram::setUniform(int location, glm::vec4 val) const { glProgramUniform4f(m_ID, location, val.x, val.y, val.z, val.w); }
void ShaderProgram::setUniform(int location, int count, const float* val) const { glProgramUniform1fv(m_ID, location, count, val); }
void ShaderProgram::setUniform(int location, int count, const glm::vec2* val) const { glProgramUniform2fv(m_ID, location, count, glm::value_ptr(*val)); }
void ShaderProgram::setUniform(int location, int count, const glm::vec3* val) const { glProgramUniform3fv(m_ID, location, count, glm::value_ptr(*val)); }
void ShaderProgram::setUniform(int location, int count, const glm::vec4* val) const { glProgramUniform4fv(m_ID, location, count, glm::value_ptr(*val)); }

void ShaderProgram::setUniform(int location, int val) const { glProgramUniform1i(m_ID, location, val); }
void ShaderProgram::setUniform(int location, glm::ivec2 val) const { glProgramUniform2i(m_ID, location, val.x, val.y); }
void ShaderProgram::setUniform(int location, glm::ivec3 val) const { glProgramUniform3i(m_ID, location, val.x, val.y, val.z); }
void ShaderProgram::setUniform(int location, glm::ivec4 val) const { glProgramUniform4i(m_ID, location, val.x, val.y, val.z, val.w); }
void ShaderProgram::setUniform(int location, int count, const int* val) const { glProgramUniform1iv(m_ID, location, count, val); }
void ShaderProgram::setUniform(int location, int count, const glm::ivec2* val) const { glProgramUniform2iv(m_ID, location, count, glm::value_ptr(*val)); }
void ShaderProgram::setUniform(int location, int count, const glm::ivec3* val) const { glProgramUniform3iv(m_ID, location, count, glm::value_ptr(*val)); }
void ShaderProgram::setUniform(int location, int count, const glm::ivec4* val) const { glProgramUniform4iv(m_ID, location, count, glm::value_ptr(*val)); }

void ShaderProgram::setUniform(int location, unsigned int val) const { glProgramUniform1ui(m_ID, location, val); }
void ShaderProgram::setUniform(int location, glm::uvec2 val) const { glProgramUniform2ui(m_ID, location, val.x, val.y); }
void ShaderProgram::setUniform(int location, glm::uvec3 val) const { glProgramUniform3ui(m_ID, location, val.x, val.y, val.z); }
void ShaderProgram::setUniform(int location, glm::uvec4 val) const { glProgramUniform4ui(m_ID, location, val.x, val.y, val.z, val.w); }
void ShaderProgram::setUniform(int location, int count, const unsigned int* val) const { glProgramUniform1uiv(m_ID, location, count, val); }
void ShaderProgram::setUniform(int location, int count, const glm::uvec2* val) const { glProgramUniform2uiv(m_ID, location, count, glm::value_ptr(*val)); }
void ShaderProgram::setUniform(int location, int count, const glm::uvec3* val) const { glProgramUniform3uiv(m_ID, location, count, glm::value_ptr(*val)); }
void ShaderProgram::setUniform(int location, int count, const glm::uvec4* val) const { glProgramUniform4uiv(m_ID, location, count, glm::value_ptr(*val)); }

void ShaderProgram::compileProgram(const ShaderProgramSource& source) {
	//Create program
	m_ID = glCreateProgram();

	//Create, compile and attach shaders
	GLuint shaderID[STAGES.size()] = {0};
	for (size_t i = 0; i < STAGES.size(); i++) {
		if (source[static_cast<ShaderType>(i)] != nullptr) {
			shaderID[i] = glCreateShader(STAGES[i]);
		#ifdef _DEBUG
			if (shaderID[i] == 0) {
				fatalError("Failed to create shader!");
			}
		#endif // _DEBUG
			compileShader(source[static_cast<ShaderType>(i)], shaderID[i]);
		}
	}

	//Link program
	linkProgram();

	//Detach and delete shaders
	for (size_t i = 0; i < STAGES.size(); i++) {
		if (shaderID[i] != 0) {
			glDetachShader(m_ID, shaderID[i]);
			glDeleteShader(shaderID[i]);
		}
	}
}

void ShaderProgram::linkProgram() {
	//Link our program
	glLinkProgram(m_ID);

	GLint isLinked = 0;
	glGetProgramiv(m_ID, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(m_ID, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(m_ID, maxLength, &maxLength, &infoLog[0]);

		std::printf("%s\n", &infoLog[0]);

		//Free shaders if not failing hard

		//Fail hard
		fatalError("Failed to link shaders!");
	}

#ifdef _DEBUG
#ifdef RE_LOG_SHADER_PROGRAM_INFO
	printInfo();
#endif // RE_LOG_SHADER_PROGRAM_INFO
	printProgramInfoLog();
#endif // _DEBUG
}

void ShaderProgram::printProgramInfoLog() const {
	GLint logLength = 0;
	glGetProgramiv(m_ID, GL_INFO_LOG_LENGTH, &logLength);

	if (logLength > 0) {//If there is a log
		std::vector<GLchar> infoLog(logLength);
		glGetProgramInfoLog(m_ID, logLength, nullptr, &infoLog[0]);

		std::cout << "|PROGRAM INFO LOG\n";
		std::printf("%s\n", &infoLog[0]);
	}
}

void ShaderProgram::compileShader(const char* source, GLuint shaderID) {
	glShaderSource(shaderID, 1, &source, nullptr);

	glCompileShader(shaderID);

	GLint compileSuccess = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileSuccess);

	if (compileSuccess == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(shaderID, maxLength, &maxLength, &infoLog[0]);

		glDeleteShader(shaderID);

		std::printf("%s\n", &infoLog[0]);

		fatalError("Failed to compile an internal shader!");
	}

	glAttachShader(m_ID, shaderID);
}

}