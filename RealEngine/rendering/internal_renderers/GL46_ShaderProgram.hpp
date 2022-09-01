/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IShaderProgram.hpp>


namespace RE {

/**
* @brief Is OpenGL 4.6 implementation of ShaderProgram class.
*
* Do not use this directly - use ShaderProgram instead.
*/
class GL46_ShaderProgram final : public IShaderProgram {
public:

    ShaderProgramInternals construct(const ShaderProgramSources& sources) const override;
    void destruct(ShaderProgramInternals& sp) const override;

    void use(const ShaderProgramInternals& sp) const override;
    void unuse(const ShaderProgramInternals& sp) const override;

    void dispatchCompute(const ShaderProgramInternals& sp, const glm::uvec3& groupCount, bool use) const override;
    void dispatchCompute(const ShaderProgramInternals& sp, int indirect, bool use) const override;

    void printInfo(const ShaderProgramInternals& sp) const override;

    void backInterfaceBlock(const ShaderProgramInternals& sp, unsigned int interfaceBlockIndex, const BufferTypedIndex& index) const override;

    int getUniformLocation(const ShaderProgramInternals& sp, const std::string& name) const override;

    void setUniform(const ShaderProgramInternals& sp, int location, float val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, float val0, float val1) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, const glm::vec2& val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, float val0, float val1, float val2) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, const glm::vec3& val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, float val0, float val1, float val2, float val3) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, const glm::vec4& val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, int count, const float* val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, int count, const glm::vec2* val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, int count, const glm::vec3* val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, int count, const glm::vec4* val) const override;

    void setUniform(const ShaderProgramInternals& sp, int location, int val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, int val0, int val1) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, const glm::ivec2& val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, int val0, int val1, int val2) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, const glm::ivec3& val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, int val0, int val1, int val2, int val3) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, const glm::ivec4& val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, int count, const int* val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, int count, const glm::ivec2* val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, int count, const glm::ivec3* val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, int count, const glm::ivec4* val) const override;

    void setUniform(const ShaderProgramInternals& sp, int location, unsigned int val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, unsigned int val0, unsigned int val1) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, const glm::uvec2& val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, unsigned int val0, unsigned int val1, unsigned int val2) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, const glm::uvec3& val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, unsigned int val0, unsigned int val1, unsigned int val2, unsigned int val3) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, const glm::uvec4& val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, int count, const unsigned int* val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, int count, const glm::uvec2* val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, int count, const glm::uvec3* val) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, int count, const glm::uvec4* val) const override;

    void setUniform(const ShaderProgramInternals& sp, int location, int count, const glm::mat4* val) const override;

    void setUniform(const ShaderProgramInternals& sp, int location, TextureUnit unit) const override;
    void setUniform(const ShaderProgramInternals& sp, int location, ImageUnit unit) const override;

private:
    //Used to compile from scratch
    ShaderProgramInternals compileProgram(const ShaderProgramSources& source) const;

    //Helper function
    void compileShader(ShaderProgramInternals& sp, const ShaderSources& source, unsigned int shaderID) const;

    //Helper function
    void linkProgram(ShaderProgramInternals& sp) const;

    //Prints program's info log
    void printProgramInfoLog(const ShaderProgramInternals& sp) const;

#ifdef _DEBUG
    static inline unsigned int s_currentlyUsedID = 0u;
#endif // _DEBUG
};

}