﻿/*!
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

    int getUniformLocation(const ShaderProgramID& sp, const std::string& name) const override;

    void setUniform(const ShaderProgramID& sp, int location, float val) const override;
    void setUniform(const ShaderProgramID& sp, int location, float val0, float val1) const override;
    void setUniform(const ShaderProgramID& sp, int location, const glm::vec2& val) const override;
    void setUniform(const ShaderProgramID& sp, int location, float val0, float val1, float val2) const override;
    void setUniform(const ShaderProgramID& sp, int location, const glm::vec3& val) const override;
    void setUniform(const ShaderProgramID& sp, int location, float val0, float val1, float val2, float val3) const override;
    void setUniform(const ShaderProgramID& sp, int location, const glm::vec4& val) const override;
    void setUniform(const ShaderProgramID& sp, int location, int count, const float* val) const override;
    void setUniform(const ShaderProgramID& sp, int location, int count, const glm::vec2* val) const override;
    void setUniform(const ShaderProgramID& sp, int location, int count, const glm::vec3* val) const override;
    void setUniform(const ShaderProgramID& sp, int location, int count, const glm::vec4* val) const override;

    void setUniform(const ShaderProgramID& sp, int location, int val) const override;
    void setUniform(const ShaderProgramID& sp, int location, int val0, int val1) const override;
    void setUniform(const ShaderProgramID& sp, int location, const glm::ivec2& val) const override;
    void setUniform(const ShaderProgramID& sp, int location, int val0, int val1, int val2) const override;
    void setUniform(const ShaderProgramID& sp, int location, const glm::ivec3& val) const override;
    void setUniform(const ShaderProgramID& sp, int location, int val0, int val1, int val2, int val3) const override;
    void setUniform(const ShaderProgramID& sp, int location, const glm::ivec4& val) const override;
    void setUniform(const ShaderProgramID& sp, int location, int count, const int* val) const override;
    void setUniform(const ShaderProgramID& sp, int location, int count, const glm::ivec2* val) const override;
    void setUniform(const ShaderProgramID& sp, int location, int count, const glm::ivec3* val) const override;
    void setUniform(const ShaderProgramID& sp, int location, int count, const glm::ivec4* val) const override;

    void setUniform(const ShaderProgramID& sp, int location, unsigned int val) const override;
    void setUniform(const ShaderProgramID& sp, int location, unsigned int val0, unsigned int val1) const override;
    void setUniform(const ShaderProgramID& sp, int location, const glm::uvec2& val) const override;
    void setUniform(const ShaderProgramID& sp, int location, unsigned int val0, unsigned int val1, unsigned int val2) const override;
    void setUniform(const ShaderProgramID& sp, int location, const glm::uvec3& val) const override;
    void setUniform(const ShaderProgramID& sp, int location, unsigned int val0, unsigned int val1, unsigned int val2, unsigned int val3) const override;
    void setUniform(const ShaderProgramID& sp, int location, const glm::uvec4& val) const override;
    void setUniform(const ShaderProgramID& sp, int location, int count, const unsigned int* val) const override;
    void setUniform(const ShaderProgramID& sp, int location, int count, const glm::uvec2* val) const override;
    void setUniform(const ShaderProgramID& sp, int location, int count, const glm::uvec3* val) const override;
    void setUniform(const ShaderProgramID& sp, int location, int count, const glm::uvec4* val) const override;

    void setUniform(const ShaderProgramID& sp, int location, int count, const glm::mat4* val) const override;

    void setUniform(const ShaderProgramID& sp, int location, TextureUnit unit) const override;
    void setUniform(const ShaderProgramID& sp, int location, ImageUnit unit) const override;

private:

    VK13ShaderProgram(const vk::Device& device) :
        m_device(device) {
    }

    const vk::Device& m_device;
};

}