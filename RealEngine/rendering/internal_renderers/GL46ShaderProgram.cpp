/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/GL46ShaderProgram.hpp>

#include <vector>
#include <iostream>
#include <array>

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <RealEngine/rendering/internal_renderers/GL46TypeToString.hpp>
#include <RealEngine/utility/Error.hpp>

namespace RE {

static GLenum convert(size_t st) {
    using enum ShaderType;
    switch (static_cast<ShaderType>(st)) {
    case VERTEX:            return GL_VERTEX_SHADER;
    case TESS_CONTROL:      return GL_TESS_CONTROL_SHADER;
    case TESS_EVALUATION:   return GL_TESS_EVALUATION_SHADER;
    case GEOMETRY:          return GL_GEOMETRY_SHADER;
    case FRAGMENT:          return GL_FRAGMENT_SHADER;
    case COMPUTE:           return GL_COMPUTE_SHADER;
    default: throw Exception{"Unknown shader type"};
    }
}

ShaderProgramID GL46ShaderProgram::construct(const ShaderProgramSources& sources) const {
    return compileProgram(sources);
}

void GL46ShaderProgram::destruct(ShaderProgramID& sp) const {
    if (sp.m_data.id != 0) {
        glDeleteProgram(sp.m_data.id);
    }
}

void GL46ShaderProgram::use(const ShaderProgramID& sp) const {
#ifndef NDEBUG
    if (s_currentlyUsedID != 0) {
        throw "Overbound shader programs";
    }
    s_currentlyUsedID = sp.m_data.id;
#endif // DEBUG
    glUseProgram(sp.m_data.id);
}

void GL46ShaderProgram::unuse(const ShaderProgramID& sp) const {
#ifndef NDEBUG
    if (s_currentlyUsedID != sp.m_data.id) {
        throw "Overbound shader programs";
    }
    s_currentlyUsedID = 0;
    glUseProgram(0);
#endif // DEBUG
}

void GL46ShaderProgram::dispatchCompute(const ShaderProgramID& sp, const glm::uvec3& groupCount, bool use) const {
    if (use) {
        this->use(sp);
    } else {
    #ifndef NDEBUG
        if (s_currentlyUsedID != sp.m_data.id) {
            throw "Tried to dispatch compute groups without having the program bound for usage!";
        }
    #endif // DEBUG
    }
    glDispatchCompute(groupCount.x, groupCount.y, groupCount.z);
    if (use) {
        unuse(sp);
    }
}

void GL46ShaderProgram::dispatchCompute(const ShaderProgramID& sp, int indirect, bool use) const {
    if (use) {
        this->use(sp);
    } else {
    #ifndef NDEBUG
        if (s_currentlyUsedID != sp.m_data.id) {
            throw "Tried to dispatch compute groups without having the program bound for usage!";
        }
    #endif // DEBUG
    }
    glDispatchComputeIndirect(indirect);
    if (use) {
        unuse(sp);
    }
}

void GL46ShaderProgram::printInfo(const ShaderProgramID& sp) const {
    std::cout << "|SHADER: " << sp.m_data.id << '\n';
    //ATTRIBUTES
    int numberAttributes;
    glGetProgramiv(sp.m_data.id, GL_ACTIVE_ATTRIBUTES, &numberAttributes);
    std::cout << "|ATTRIBUTES: " << std::to_string(numberAttributes) << "\n";
    for (int i = 0; i < numberAttributes; i++) {
        GLsizei length;
        int size;
        GLenum type;
        GLchar name[32];
        glGetActiveAttrib(sp.m_data.id, i, 32, &length, &size, &type, &name[0]);
        int loc = glGetAttribLocation(sp.m_data.id, &name[0]);
        std::cout << "|  layout (location = " << std::to_string(loc) << ") in " << GL46TypeToString(type) << " " << name << ((size > 1) ? ("[" + std::to_string(size) + "]") : "") << '\n';
    }

    //UNIFORMS
    int numberUniforms;
    glGetProgramiv(sp.m_data.id, GL_ACTIVE_UNIFORMS, &numberUniforms);
    std::cout << "|UNIFORMS: " << std::to_string(numberUniforms) << "\n";
    for (int i = 0; i < numberUniforms; i++) {
        GLsizei length;
        int size;
        GLenum type;
        GLchar name[32];
        glGetActiveUniform(sp.m_data.id, i, 32, &length, &size, &type, &name[0]);
        int loc = glGetUniformLocation(sp.m_data.id, &name[0]);
        std::printf("|  layout (location = %i) uniform %s %s%s;\n",
            loc, &(GL46TypeToString(type))[0], name, ((size > 1) ? ("[" + std::to_string(size) + "]") : "").c_str());
    }

    //UNIFORM BLOCKS
    GLint numberUniformBlocks;
    glGetProgramiv(sp.m_data.id, GL_ACTIVE_UNIFORM_BLOCKS, &numberUniformBlocks);
    std::cout << "|UNIFORM BLOCKS: " << std::to_string(numberUniformBlocks) << "\n";
    for (int i = 0; i < numberUniformBlocks; i++) {
        GLint nameLength;
        glGetActiveUniformBlockiv(sp.m_data.id, i, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLength);
        GLchar name[32];
        glGetActiveUniformBlockName(sp.m_data.id, i, 32, nullptr, name);
        GLuint binding = glGetUniformBlockIndex(sp.m_data.id, name);
        GLint currBoundTo;
        glGetActiveUniformBlockiv(sp.m_data.id, i, GL_UNIFORM_BLOCK_BINDING, &currBoundTo);
        GLint dataSize;
        glGetActiveUniformBlockiv(sp.m_data.id, i, GL_UNIFORM_BLOCK_DATA_SIZE, &dataSize);
        GLint numberActiveUniforms;
        glGetActiveUniformBlockiv(sp.m_data.id, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &numberActiveUniforms);
        std::printf("|  layout (binding = %u (currBoundTo = %i)) uniform %s {%i active uniforms (totalSize = %i)};\n",
            binding, currBoundTo, name, numberActiveUniforms, dataSize);
    }
}

void GL46ShaderProgram::backInterfaceBlock(const ShaderProgramID& sp, unsigned int interfaceBlockIndex, const BufferTypedIndex& index) const {
    if (index.type == BufferType::UNIFORM) {
        glUniformBlockBinding(sp.m_data.id, interfaceBlockIndex, index.bindingIndex);
    } else if (index.type == BufferType::SHADER_STORAGE) {
        glShaderStorageBlockBinding(sp.m_data.id, interfaceBlockIndex, index.bindingIndex);
    }
#ifndef NDEBUG
    else {
        throw "Interface blocks must be either UNIFORM or SHADER_STORAGE";
    }
#endif // DEBUG
}

void GL46ShaderProgram::setUniform(const ShaderProgramID& sp, int location, TextureUnit unit) const { glProgramUniform1i(sp.m_data.id, location, unit.m_unit); }
void GL46ShaderProgram::setUniform(const ShaderProgramID& sp, int location, ImageUnit unit) const { glProgramUniform1i(sp.m_data.id, location, unit.m_unit); }

ShaderProgramID GL46ShaderProgram::compileProgram(const ShaderProgramSources& sources) const {
    //Create program
    auto sp = ShaderProgramID(glCreateProgram());

    //Create, compile and attach shaders
    std::array<unsigned int, ShaderProgramSources::NUM_STAGES> shaderIDs{};
    for (size_t i = 0; i < ShaderProgramSources::NUM_STAGES; ++i) {
        if (!sources[i].gl46.empty()) {
            shaderIDs[i] = glCreateShader(convert(i));
        #ifndef NDEBUG
            if (shaderIDs[i] == 0) {
                fatalError("Failed to create shader!");
            }
        #endif // DEBUG
            compileShader(sp, sources[i], shaderIDs[i]);
        }
    }

    //Link program
    linkProgram(sp);

    //Detach and delete shaders
    for (size_t i = 0; i < 6; i++) {
        if (shaderIDs[i] != 0) {
            glDetachShader(sp.m_data.id, shaderIDs[i]);
            glDeleteShader(shaderIDs[i]);
        }
    }

    return sp;
}

void GL46ShaderProgram::compileShader(ShaderProgramID& sp, const ShaderSourceRef& source, unsigned int shaderID) const {
    glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, source.gl46.data(), static_cast<GLsizei>(source.gl46.size() * sizeof(uint32_t)));

    glSpecializeShader(shaderID, "main", 0u, nullptr, nullptr);

    int compileSuccess = 0;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileSuccess);

    if (compileSuccess == GL_FALSE) {
        int maxLength = 0;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

        if (maxLength > 0) {
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(shaderID, maxLength, &maxLength, infoLog.data());
            error(infoLog.data());
        }

        glDeleteShader(shaderID);
        fatalError("Failed to compile an internal shader!");
    }

    glAttachShader(sp.m_data.id, shaderID);
}

void GL46ShaderProgram::linkProgram(ShaderProgramID& sp) const {
    glLinkProgram(sp.m_data.id);

    int isLinked = 0;
    glGetProgramiv(sp.m_data.id, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        int maxLength = 0;
        glGetProgramiv(sp.m_data.id, GL_INFO_LOG_LENGTH, &maxLength);

        if (maxLength > 0) {
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(sp.m_data.id, maxLength, &maxLength, infoLog.data());
            error(infoLog.data());
        }

        fatalError("Failed to link shaders!");
    }

#ifndef NDEBUG
    printProgramInfoLog(sp);
#endif // DEBUG
}

void GL46ShaderProgram::printProgramInfoLog(const ShaderProgramID& sp) const {
    int logLength = 0;
    glGetProgramiv(sp.m_data.id, GL_INFO_LOG_LENGTH, &logLength);

    if (logLength > 0) {//If there is a log
        std::vector<GLchar> infoLog(logLength);
        glGetProgramInfoLog(sp.m_data.id, logLength, nullptr, &infoLog[0]);

        std::cout << "|PROGRAM INFO LOG\n";
        std::printf("%s\n", &infoLog[0]);
    }
}

}