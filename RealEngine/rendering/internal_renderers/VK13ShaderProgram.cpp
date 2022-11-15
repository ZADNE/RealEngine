/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/VK13ShaderProgram.hpp>

#include <array>

#include <RealEngine/window/VK13Fixture.hpp>

namespace RE {

static vk::ShaderStageFlagBits convert(size_t st) {
    using enum ShaderType;
    switch (static_cast<ShaderType>(st)) {
    case VERTEX:            return vk::ShaderStageFlagBits::eVertex;
    case TESS_CONTROL:      return vk::ShaderStageFlagBits::eTessellationControl;
    case TESS_EVALUATION:   return vk::ShaderStageFlagBits::eTessellationEvaluation;
    case GEOMETRY:          return vk::ShaderStageFlagBits::eGeometry;
    case FRAGMENT:          return vk::ShaderStageFlagBits::eFragment;
    case COMPUTE:           return vk::ShaderStageFlagBits::eCompute;
    default: throw Exception{"Unknown shader type"};
    }
}

ShaderProgramID VK13ShaderProgram::construct(const ShaderProgramSources& sources) const {
    std::array<vk::ShaderModule, 6> modules;
    std::array<vk::PipelineShaderStageCreateInfo, 6> stages;
    size_t shaderCount = 0;
    for (size_t st = 0; st < ShaderProgramSources::NUM_STAGES; ++st) {
        if (!sources[st].vk13.empty()) {
            modules[shaderCount] = m_device.createShaderModule({
                vk::ShaderModuleCreateFlags{},
                sources[st].vk13.size() * 4,
                sources[st].vk13.data()}
            );
            stages[shaderCount] = vk::PipelineShaderStageCreateInfo{
                vk::PipelineShaderStageCreateFlags{},
                convert(st),
                modules[shaderCount],
                "main"
            };
            shaderCount++;
        }
    }

    return ShaderProgramID{0u};
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