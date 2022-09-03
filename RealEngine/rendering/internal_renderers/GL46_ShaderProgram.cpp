/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/GL46_ShaderProgram.hpp>

#include <iostream>
#include <array>

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <RealEngine/utility/GLTypeToString.hpp>
#include <RealEngine/utility/error.hpp>
#include <RealEngine/rendering/vertices/ShaderProgram.hpp>

namespace RE {

using enum ShaderType;

const std::array SHADER_STAGES = {
    ShaderType::VERTEX,
    ShaderType::TESS_CONTROL,
    ShaderType::TESS_EVALUATION,
    ShaderType::GEOMETRY,
    ShaderType::FRAGMENT,
    ShaderType::COMPUTE
};

GLenum convert(ShaderType st) {
    switch (st) {
    case VERTEX:            return GL_VERTEX_SHADER;
    case TESS_CONTROL:      return GL_TESS_CONTROL_SHADER;
    case TESS_EVALUATION:   return GL_TESS_EVALUATION_SHADER;
    case GEOMETRY:          return GL_GEOMETRY_SHADER;
    case FRAGMENT:          return GL_FRAGMENT_SHADER;
    case COMPUTE:           return GL_COMPUTE_SHADER;
    default: throw "Unknown shader type!";
    }
}

ShaderProgramID GL46_ShaderProgram::construct(const ShaderProgramSources& sources) const {
    return compileProgram(sources);
}

void GL46_ShaderProgram::destruct(ShaderProgramID& sp) const {
    if (sp.m_id != 0) {
        glDeleteProgram(sp.m_id);
    }
}

void GL46_ShaderProgram::use(const ShaderProgramID& sp) const {
#ifdef _DEBUG
    if (s_currentlyUsedID != 0) {
        throw "Overbound shader programs";
    }
    s_currentlyUsedID = sp.m_id;
#endif // _DEBUG
    glUseProgram(sp.m_id);
}

void GL46_ShaderProgram::unuse(const ShaderProgramID& sp) const {
#ifdef _DEBUG
    if (s_currentlyUsedID != sp.m_id) {
        throw "Overbound shader programs";
    }
    s_currentlyUsedID = 0;
    glUseProgram(0);
#endif // _DEBUG
}

void GL46_ShaderProgram::dispatchCompute(const ShaderProgramID& sp, const glm::uvec3& groupCount, bool use) const {
    if (use) {
        this->use(sp);
    } else {
    #ifdef _DEBUG
        if (s_currentlyUsedID != sp.m_id) {
            throw "Tried to dispatch compute groups without having the program bound for usage!";
        }
    #endif // _DEBUG
    }
    glDispatchCompute(groupCount.x, groupCount.y, groupCount.z);
    if (use) {
        unuse(sp);
    }
}

void GL46_ShaderProgram::dispatchCompute(const ShaderProgramID& sp, int indirect, bool use) const {
    if (use) {
        this->use(sp);
    } else {
    #ifdef _DEBUG
        if (s_currentlyUsedID != sp.m_id) {
            throw "Tried to dispatch compute groups without having the program bound for usage!";
        }
    #endif // _DEBUG
    }
    glDispatchComputeIndirect(indirect);
    if (use) {
        unuse(sp);
    }
}

void GL46_ShaderProgram::printInfo(const ShaderProgramID& sp) const {
    std::cout << "|SHADER: " << sp.m_id << '\n';
    //ATTRIBUTE INFO
    int numberAttributes;
    glGetProgramiv(sp.m_id, GL_ACTIVE_ATTRIBUTES, &numberAttributes);
    std::cout << "|ATTRIBUTES: " << std::to_string(numberAttributes) << "\n";
    for (int i = 0; i < numberAttributes; i++) {
        GLsizei length;
        int size;
        GLenum type;
        GLchar name[32];
        glGetActiveAttrib(sp.m_id, i, 32, &length, &size, &type, &name[0]);
        int loc = glGetAttribLocation(sp.m_id, &name[0]);
        std::cout << "|  layout (location = " << std::to_string(loc) << ") in " << GLTypeToString(type) << " " << name << ((size > 1) ? ("[" + std::to_string(size) + "]") : "") << '\n';
    }

    //UNIFORM INFO
    int numberUniforms;
    glGetProgramiv(sp.m_id, GL_ACTIVE_UNIFORMS, &numberUniforms);
    std::cout << "|UNIFORMS: " << std::to_string(numberUniforms) << "\n";
    for (int i = 0; i < numberUniforms; i++) {
        GLsizei length;
        int size;
        GLenum type;
        GLchar name[32];
        glGetActiveUniform(sp.m_id, i, 32, &length, &size, &type, &name[0]);
        int loc = glGetUniformLocation(sp.m_id, &name[0]);
        std::cout << "|  layout (location = " << std::to_string(loc) << ") uniform " << GLTypeToString(type) << " " << name << ((size > 1) ? ("[" + std::to_string(size) + "]") : "") << '\n';
    }
}

void GL46_ShaderProgram::backInterfaceBlock(const ShaderProgramID& sp, unsigned int interfaceBlockIndex, const BufferTypedIndex& index) const {
    if (index.type == RE::BufferType::UNIFORM) {
        glUniformBlockBinding(sp.m_id, interfaceBlockIndex, index.bindingIndex);
    } else if (index.type == RE::BufferType::SHADER_STORAGE) {
        glShaderStorageBlockBinding(sp.m_id, interfaceBlockIndex, index.bindingIndex);
    }
#ifdef _DEBUG
    else {
        throw "Interface blocks must be either UNIFORM or SHADER_STORAGE";
    }
#endif // _DEBUG
}

int GL46_ShaderProgram::getUniformLocation(const ShaderProgramID& sp, const std::string& name) const {
    return glGetUniformLocation(sp.m_id, name.c_str());
}

void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, float val) const { glProgramUniform1f(sp.m_id, location, val); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, float val0, float val1) const { glProgramUniform2f(sp.m_id, location, val0, val1); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, const glm::vec2& val) const { setUniform(sp, location, val.x, val.y); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, float val0, float val1, float val2) const { glProgramUniform3f(sp.m_id, location, val0, val1, val2); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, const glm::vec3& val) const { setUniform(sp, location, val.x, val.y, val.z); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, float val0, float val1, float val2, float val3) const { glProgramUniform4f(sp.m_id, location, val0, val1, val2, val3); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, const glm::vec4& val) const { setUniform(sp, location, val.x, val.y, val.z, val.w); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const float* val) const { glProgramUniform1fv(sp.m_id, location, count, val); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const glm::vec2* val) const { glProgramUniform2fv(sp.m_id, location, count, glm::value_ptr(*val)); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const glm::vec3* val) const { glProgramUniform3fv(sp.m_id, location, count, glm::value_ptr(*val)); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const glm::vec4* val) const { glProgramUniform4fv(sp.m_id, location, count, glm::value_ptr(*val)); }

void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int val) const { glProgramUniform1i(sp.m_id, location, val); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int val0, int val1) const { glProgramUniform2i(sp.m_id, location, val0, val1); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, const glm::ivec2& val) const { setUniform(sp, location, val.x, val.y); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int val0, int val1, int val2) const { glProgramUniform3i(sp.m_id, location, val0, val1, val2); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, const glm::ivec3& val) const { setUniform(sp, location, val.x, val.y, val.z); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int val0, int val1, int val2, int val3) const { glProgramUniform4i(sp.m_id, location, val0, val1, val2, val3); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, const glm::ivec4& val) const { setUniform(sp, location, val.x, val.y, val.z, val.w); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const int* val) const { glProgramUniform1iv(sp.m_id, location, count, val); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const glm::ivec2* val) const { glProgramUniform2iv(sp.m_id, location, count, glm::value_ptr(*val)); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const glm::ivec3* val) const { glProgramUniform3iv(sp.m_id, location, count, glm::value_ptr(*val)); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const glm::ivec4* val) const { glProgramUniform4iv(sp.m_id, location, count, glm::value_ptr(*val)); }

void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, unsigned int val) const { glProgramUniform1ui(sp.m_id, location, val); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, unsigned int val0, unsigned int val1) const { glProgramUniform2ui(sp.m_id, location, val0, val1); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, const glm::uvec2& val) const { setUniform(sp, location, val.x, val.y); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, unsigned int val0, unsigned int val1, unsigned int val2) const { glProgramUniform3ui(sp.m_id, location, val0, val1, val2); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, const glm::uvec3& val) const { setUniform(sp, location, val.x, val.y, val.z); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, unsigned int val0, unsigned int val1, unsigned int val2, unsigned int val3) const { glProgramUniform4ui(sp.m_id, location, val0, val1, val2, val3); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, const glm::uvec4& val) const { setUniform(sp, location, val.x, val.y, val.z, val.w); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const unsigned int* val) const { glProgramUniform1uiv(sp.m_id, location, count, val); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const glm::uvec2* val) const { glProgramUniform2uiv(sp.m_id, location, count, glm::value_ptr(*val)); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const glm::uvec3* val) const { glProgramUniform3uiv(sp.m_id, location, count, glm::value_ptr(*val)); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const glm::uvec4* val) const { glProgramUniform4uiv(sp.m_id, location, count, glm::value_ptr(*val)); }

void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const glm::mat4* val) const { glProgramUniformMatrix4fv(sp.m_id, location, count, false, glm::value_ptr(*val)); }

void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, TextureUnit unit) const { glProgramUniform1i(sp.m_id, location, unit.m_unit); }
void GL46_ShaderProgram::setUniform(const ShaderProgramID& sp, int location, ImageUnit unit) const { glProgramUniform1i(sp.m_id, location, unit.m_unit); }

ShaderProgramID GL46_ShaderProgram::compileProgram(const ShaderProgramSources& source) const {
    //Create program
    auto sp = ShaderProgramID(glCreateProgram());

    //Create, compile and attach shaders
    unsigned int shaderID[SHADER_STAGES.size()] = {0};
    size_t i = 0;
    for (auto STAGE : SHADER_STAGES) {
        if (!source[STAGE].m_sources.empty()) {
            shaderID[i] = glCreateShader(convert(STAGE));
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
            glDetachShader(sp.m_id, shaderID[i]);
            glDeleteShader(shaderID[i]);
        }
    }

    return sp;
}

void GL46_ShaderProgram::compileShader(ShaderProgramID& sp, const ShaderSources& sources, unsigned int shaderID) const {
    glShaderSource(shaderID, static_cast<GLsizei>(sources.m_sources.size()), sources.m_sources.data(), sources.m_lengths.data());

    glCompileShader(shaderID);

    int compileSuccess = 0;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileSuccess);

    if (compileSuccess == GL_FALSE) {
        int maxLength = 0;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(shaderID, maxLength, &maxLength, &infoLog[0]);

        glDeleteShader(shaderID);

        std::printf("%s\n", &infoLog[0]);

        fatalError("Failed to compile an internal shader!");
    }

    glAttachShader(sp.m_id, shaderID);
}

void GL46_ShaderProgram::linkProgram(ShaderProgramID& sp) const {
    //Link our program
    glLinkProgram(sp.m_id);

    int isLinked = 0;
    glGetProgramiv(sp.m_id, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        int maxLength = 0;
        glGetProgramiv(sp.m_id, GL_INFO_LOG_LENGTH, &maxLength);

        //The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(sp.m_id, maxLength, &maxLength, &infoLog[0]);

        std::printf("%s\n", &infoLog[0]);

        //Free shaders if not failing hard

        //Fail hard
        fatalError("Failed to link shaders!");
    }

#ifdef _DEBUG
    printProgramInfoLog(sp);
#endif // _DEBUG
}

void GL46_ShaderProgram::printProgramInfoLog(const ShaderProgramID& sp) const {
    int logLength = 0;
    glGetProgramiv(sp.m_id, GL_INFO_LOG_LENGTH, &logLength);

    if (logLength > 0) {//If there is a log
        std::vector<GLchar> infoLog(logLength);
        glGetProgramInfoLog(sp.m_id, logLength, nullptr, &infoLog[0]);

        std::cout << "|PROGRAM INFO LOG\n";
        std::printf("%s\n", &infoLog[0]);
    }
}

}