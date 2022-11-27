/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/VK13ShaderProgram.hpp>

#include <array>

#include <RealEngine/window/VK13Fixture.hpp>

namespace RE {

ShaderProgramID VK13ShaderProgram::construct(const ShaderProgramSources& sources) const {
    return 0;
}

void VK13ShaderProgram::destruct(ShaderProgramID& sp) const {

}

void VK13ShaderProgram::use(const ShaderProgramID& sp) const {

}

void VK13ShaderProgram::unuse(const ShaderProgramID& sp) const {

}

void VK13ShaderProgram::dispatchCompute(const ShaderProgramID& sp, const glm::uvec3& groupCount, bool use) const {

}

void VK13ShaderProgram::dispatchCompute(const ShaderProgramID& sp, int indirect, bool use) const {

}

void VK13ShaderProgram::printInfo(const ShaderProgramID& sp) const {

}

void VK13ShaderProgram::backInterfaceBlock(const ShaderProgramID& sp, unsigned int interfaceBlockIndex, const BufferTypedIndex& index) const {

}

void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, TextureUnit unit) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, ImageUnit unit) const {  }

}