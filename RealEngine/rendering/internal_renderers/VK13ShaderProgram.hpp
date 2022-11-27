/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IShaderProgram.hpp>


namespace RE {

/**
* @brief Is Vulkan 1.3 implementation of ShaderProgram class.
*
* Do not use this directly - use ShaderProgram instead.
*/
class VK13ShaderProgram final : public IShaderProgram {
    friend class VK13Fixture;
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
};

}