/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/internal_renderers/GL46_ShaderProgram.hpp>

#include <iostream>
#include <array>

#include <glm/gtc/type_ptr.hpp>

#include <RealEngine/utility/GLTypeToString.hpp>
#include <RealEngine/main/Error.hpp>
#include <RealEngine/graphics/ShaderProgram.hpp>

namespace RE {

const std::array SHADER_STAGES = {
	ShaderType::VERTEX,
	ShaderType::TESS_CONTROL,
	ShaderType::TESS_EVALUATION,
	ShaderType::GEOMETRY,
	ShaderType::FRAGMENT,
	ShaderType::COMPUTE
};

void GL46_ShaderProgram::construct(ShaderProgram& sp, const ShaderProgramSources& sources) const {
	compileProgram(sp, sources);
#if defined(_DEBUG) && defined(RE_LOG_SHADER_PROGRAM_CREATED)
	std::cout << "Created shader (ID: " << sp.m_ID << ")\n";
#endif // defined(_DEBUG) && defined(RE_LOG_SHADER_PROGRAM_CREATED)
}

void GL46_ShaderProgram::destruct(ShaderProgram& sp) const {
	if (sp.m_ID != 0) {
	#if defined(_DEBUG) && defined(RE_LOG_SHADER_PROGRAM_DESTROYED)
		std::cout << "Destroyed shader (ID: " << sp.m_ID << ")\n";
	#endif // defined(_DEBUG) && defined(RE_LOG_SHADER_PROGRAM_DESTROYED)
		glDeleteProgram(sp.m_ID);
	}
}

void GL46_ShaderProgram::use(const ShaderProgram& sp) const {
#ifdef _DEBUG
	if (s_currentlyUsedID != 0) {
		throw "Overbound shader programs";
	}
	s_currentlyUsedID = sp.m_ID;
#endif // _DEBUG
	glUseProgram(sp.m_ID);
}

void GL46_ShaderProgram::unuse(const ShaderProgram& sp) const {
#ifdef _DEBUG
	if (s_currentlyUsedID != sp.m_ID) {
		throw "Overbound shader programs";
	}
	s_currentlyUsedID = 0;
	glUseProgram(0);
#endif // _DEBUG
}

void GL46_ShaderProgram::dispatchCompute(const ShaderProgram& sp, const glm::uvec3& groupCount, bool use) const {
	if (use) {
		this->use(sp);
	} else {
	#ifdef _DEBUG
		if (s_currentlyUsedID != sp.m_ID) {
			throw "Tried to dispatch compute groups without having the program bound for usage!";
		}
	#endif // _DEBUG
	}
	glDispatchCompute(groupCount.x, groupCount.y, groupCount.z);
	if (use) {
		unuse(sp);
	}
}

void GL46_ShaderProgram::dispatchCompute(const ShaderProgram& sp, GLintptr indirect, bool use) const {
	if (use) {
		this->use(sp);
	} else {
	#ifdef _DEBUG
		if (s_currentlyUsedID != sp.m_ID) {
			throw "Tried to dispatch compute groups without having the program bound for usage!";
		}
	#endif // _DEBUG
	}
	glDispatchComputeIndirect(indirect);
	if (use) {
		unuse(sp);
	}
}

void GL46_ShaderProgram::printInfo(const ShaderProgram& sp) const {
	std::cout << "|SHADER: " << sp.m_ID << '\n';
	//ATTRIBUTE INFO
	GLint numberAttributes;
	glGetProgramiv(sp.m_ID, GL_ACTIVE_ATTRIBUTES, &numberAttributes);
	std::cout << "|ATTRIBUTES: " << std::to_string(numberAttributes) << "\n";
	for (GLint i = 0; i < numberAttributes; i++) {
		GLsizei length;
		GLint size;
		GLenum type;
		GLchar name[32];
		glGetActiveAttrib(sp.m_ID, i, 32, &length, &size, &type, &name[0]);
		GLint loc = glGetAttribLocation(sp.m_ID, &name[0]);
		std::cout << "|  layout (location = " << std::to_string(loc) << ") in " << GLTypeToString(type) << " " << name << ((size > 1) ? ("[" + std::to_string(size) + "]") : "") << '\n';
	}

	//UNIFORM INFO
	GLint numberUniforms;
	glGetProgramiv(sp.m_ID, GL_ACTIVE_UNIFORMS, &numberUniforms);
	std::cout << "|UNIFORMS: " << std::to_string(numberUniforms) << "\n";
	for (GLint i = 0; i < numberUniforms; i++) {
		GLsizei length;
		GLint size;
		GLenum type;
		GLchar name[32];
		glGetActiveUniform(sp.m_ID, i, 32, &length, &size, &type, &name[0]);
		GLint loc = glGetUniformLocation(sp.m_ID, &name[0]);
		std::cout << "|  layout (location = " << std::to_string(loc) << ") uniform " << GLTypeToString(type) << " " << name << ((size > 1) ? ("[" + std::to_string(size) + "]") : "") << '\n';
	}
}

void GL46_ShaderProgram::backInterfaceBlock(const ShaderProgram& sp, GLuint interfaceBlockIndex, const BufferTypedIndex& index) const {
	if (index.type == RE::BufferType::UNIFORM) {
		glUniformBlockBinding(sp.m_ID, interfaceBlockIndex, index.bindingIndex);
	} else if (index.type == RE::BufferType::SHADER_STORAGE) {
		glShaderStorageBlockBinding(sp.m_ID, interfaceBlockIndex, index.bindingIndex);
	}
#ifdef _DEBUG
	else {
		throw "Interface blocks must be either UNIFORM or SHADER_STORAGE";
	}
#endif // _DEBUG
}

int GL46_ShaderProgram::getUniformLocation(const ShaderProgram& sp, const std::string& name) const {
	return glGetUniformLocation(sp.m_ID, name.c_str());
}

void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, float val) const { glProgramUniform1f(sp.m_ID, location, val); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, float val0, float val1) const { glProgramUniform2f(sp.m_ID, location, val0, val1); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, const glm::vec2& val) const { setUniform(sp, location, val.x, val.y); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, float val0, float val1, float val2) const { glProgramUniform3f(sp.m_ID, location, val0, val1, val2); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, const glm::vec3& val) const { setUniform(sp, location, val.x, val.y, val.z); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, float val0, float val1, float val2, float val3) const { glProgramUniform4f(sp.m_ID, location, val0, val1, val2, val3); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, const glm::vec4& val) const { setUniform(sp, location, val.x, val.y, val.z, val.w); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, int count, const float* val) const { glProgramUniform1fv(sp.m_ID, location, count, val); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, int count, const glm::vec2* val) const { glProgramUniform2fv(sp.m_ID, location, count, glm::value_ptr(*val)); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, int count, const glm::vec3* val) const { glProgramUniform3fv(sp.m_ID, location, count, glm::value_ptr(*val)); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, int count, const glm::vec4* val) const { glProgramUniform4fv(sp.m_ID, location, count, glm::value_ptr(*val)); }

void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, GLint val) const { glProgramUniform1i(sp.m_ID, location, val); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, GLint val0, GLint val1) const { glProgramUniform2i(sp.m_ID, location, val0, val1); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, const glm::ivec2& val) const { setUniform(sp, location, val.x, val.y); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, GLint val0, GLint val1, GLint val2) const { glProgramUniform3i(sp.m_ID, location, val0, val1, val2); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, const glm::ivec3& val) const { setUniform(sp, location, val.x, val.y, val.z); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, GLint val0, GLint val1, GLint val2, GLint val3) const { glProgramUniform4i(sp.m_ID, location, val0, val1, val2, val3); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, const glm::ivec4& val) const { setUniform(sp, location, val.x, val.y, val.z, val.w); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, int count, const GLint* val) const { glProgramUniform1iv(sp.m_ID, location, count, val); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, int count, const glm::ivec2* val) const { glProgramUniform2iv(sp.m_ID, location, count, glm::value_ptr(*val)); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, int count, const glm::ivec3* val) const { glProgramUniform3iv(sp.m_ID, location, count, glm::value_ptr(*val)); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, int count, const glm::ivec4* val) const { glProgramUniform4iv(sp.m_ID, location, count, glm::value_ptr(*val)); }

void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, GLuint val) const { glProgramUniform1ui(sp.m_ID, location, val); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, GLuint val0, GLuint val1) const { glProgramUniform2ui(sp.m_ID, location, val0, val1); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, const glm::uvec2& val) const { setUniform(sp, location, val.x, val.y); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, GLuint val0, GLuint val1, GLuint val2) const { glProgramUniform3ui(sp.m_ID, location, val0, val1, val2); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, const glm::uvec3& val) const { setUniform(sp, location, val.x, val.y, val.z); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, GLuint val0, GLuint val1, GLuint val2, GLuint val3) const { glProgramUniform4ui(sp.m_ID, location, val0, val1, val2, val3); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, const glm::uvec4& val) const { setUniform(sp, location, val.x, val.y, val.z, val.w); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, int count, const GLuint* val) const { glProgramUniform1uiv(sp.m_ID, location, count, val); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, int count, const glm::uvec2* val) const { glProgramUniform2uiv(sp.m_ID, location, count, glm::value_ptr(*val)); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, int count, const glm::uvec3* val) const { glProgramUniform3uiv(sp.m_ID, location, count, glm::value_ptr(*val)); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, int count, const glm::uvec4* val) const { glProgramUniform4uiv(sp.m_ID, location, count, glm::value_ptr(*val)); }

void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, int count, const glm::mat4* val) const { glProgramUniformMatrix4fv(sp.m_ID, location, count, false, glm::value_ptr(*val)); }

void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, TextureUnit unit) const { glProgramUniform1i(sp.m_ID, location, unit.m_unit); }
void GL46_ShaderProgram::setUniform(const ShaderProgram& sp, int location, ImageUnit unit) const { glProgramUniform1i(sp.m_ID, location, unit.m_unit); }

void GL46_ShaderProgram::compileProgram(ShaderProgram& sp, const ShaderProgramSources& source) const {
	//Create program
	sp.m_ID = glCreateProgram();

	//Create, compile and attach shaders
	GLuint shaderID[SHADER_STAGES.size()] = {0};
	size_t i = 0;
	for (auto STAGE : SHADER_STAGES) {
		if (!source[STAGE].m_sources.empty()) {
			shaderID[i] = glCreateShader(static_cast<GLenum>(STAGE));
		#ifdef _DEBUG
			if (shaderID[i] == 0) {
				fatalError("Failed to create shader!");
			}
		#endif // _DEBUG
			compileShader(sp, source[STAGE], shaderID[i]);
		}
		i++;
	}

	//Link program
	linkProgram(sp);

	//Detach and delete shaders
	for (size_t i = 0; i < 6; i++) {
		if (shaderID[i] != 0) {
			glDetachShader(sp.m_ID, shaderID[i]);
			glDeleteShader(shaderID[i]);
		}
	}
}

void GL46_ShaderProgram::compileShader(ShaderProgram& sp, const ShaderSources& sources, GLuint shaderID) const {
	glShaderSource(shaderID, static_cast<GLsizei>(sources.m_sources.size()), sources.m_sources.data(), sources.m_lengths.data());

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

	glAttachShader(sp.m_ID, shaderID);
}

void GL46_ShaderProgram::linkProgram(ShaderProgram& sp) const {
	//Link our program
	glLinkProgram(sp.m_ID);

	GLint isLinked = 0;
	glGetProgramiv(sp.m_ID, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(sp.m_ID, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(sp.m_ID, maxLength, &maxLength, &infoLog[0]);

		std::printf("%s\n", &infoLog[0]);

		//Free shaders if not failing hard

		//Fail hard
		fatalError("Failed to link shaders!");
	}

#ifdef _DEBUG
#ifdef RE_LOG_SHADER_PROGRAM_INFO
	printInfo();
#endif // RE_LOG_SHADER_PROGRAM_INFO
	printProgramInfoLog(sp);
#endif // _DEBUG
}

void GL46_ShaderProgram::printProgramInfoLog(const ShaderProgram& sp) const {
	GLint logLength = 0;
	glGetProgramiv(sp.m_ID, GL_INFO_LOG_LENGTH, &logLength);

	if (logLength > 0) {//If there is a log
		std::vector<GLchar> infoLog(logLength);
		glGetProgramInfoLog(sp.m_ID, logLength, nullptr, &infoLog[0]);

		std::cout << "|PROGRAM INFO LOG\n";
		std::printf("%s\n", &infoLog[0]);
	}
}

}