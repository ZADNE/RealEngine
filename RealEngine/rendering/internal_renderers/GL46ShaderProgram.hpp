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
class GL46ShaderProgram final : public IShaderProgram {
public:

    ShaderProgramID construct(const ShaderProgramSources& sources) const override;
    void destruct(ShaderProgramID& sp) const override;

    void use(const ShaderProgramID& sp) const override;
    void unuse(const ShaderProgramID& sp) const override;

    void dispatchCompute(const ShaderProgramID& sp, const glm::uvec3& groupCount, bool use) const override;
    void dispatchCompute(const ShaderProgramID& sp, int indirect, bool use) const override;

    void printInfo(const ShaderProgramID& sp) const override;

    void backInterfaceBlock(const ShaderProgramID& sp, unsigned int interfaceBlockIndex, const BufferTypedIndex& index) const override;

    void setUniform(const ShaderProgramID& sp, int location, TextureUnit unit) const override;
    void setUniform(const ShaderProgramID& sp, int location, ImageUnit unit) const override;

private:

    //Used to compile from scratch
    ShaderProgramID compileProgram(const ShaderProgramSources& source) const;

    //Helper function
    void compileShader(ShaderProgramID& sp, const ShaderSourceRef& source, unsigned int shaderID) const;

    //Helper function
    void linkProgram(ShaderProgramID& sp) const;

    //Prints program's info log
    void printProgramInfoLog(const ShaderProgramID& sp) const;

#ifndef NDEBUG
    static inline unsigned int s_currentlyUsedID = 0u;
#endif // DEBUG
};

}