/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/VK13ShaderProgram.hpp>

namespace RE {

using enum ShaderType;

ShaderProgramID VK13ShaderProgram::construct(const ShaderProgramSources& sources) const {
    return ShaderProgramID{0};
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

int VK13ShaderProgram::getUniformLocation(const ShaderProgramID& sp, const std::string& name) const {
    return -1;
}

void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, float val) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, float val0, float val1) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, const glm::vec2& val) const { setUniform(sp, location, val.x, val.y); }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, float val0, float val1, float val2) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, const glm::vec3& val) const { setUniform(sp, location, val.x, val.y, val.z); }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, float val0, float val1, float val2, float val3) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, const glm::vec4& val) const { setUniform(sp, location, val.x, val.y, val.z, val.w); }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const float* val) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const glm::vec2* val) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const glm::vec3* val) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const glm::vec4* val) const {  }

void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int val) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int val0, int val1) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, const glm::ivec2& val) const { setUniform(sp, location, val.x, val.y); }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int val0, int val1, int val2) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, const glm::ivec3& val) const { setUniform(sp, location, val.x, val.y, val.z); }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int val0, int val1, int val2, int val3) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, const glm::ivec4& val) const { setUniform(sp, location, val.x, val.y, val.z, val.w); }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const int* val) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const glm::ivec2* val) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const glm::ivec3* val) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const glm::ivec4* val) const {  }

void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, unsigned int val) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, unsigned int val0, unsigned int val1) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, const glm::uvec2& val) const { setUniform(sp, location, val.x, val.y); }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, unsigned int val0, unsigned int val1, unsigned int val2) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, const glm::uvec3& val) const { setUniform(sp, location, val.x, val.y, val.z); }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, unsigned int val0, unsigned int val1, unsigned int val2, unsigned int val3) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, const glm::uvec4& val) const { setUniform(sp, location, val.x, val.y, val.z, val.w); }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const unsigned int* val) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const glm::uvec2* val) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const glm::uvec3* val) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const glm::uvec4* val) const {  }

void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, int count, const glm::mat4* val) const {  }

void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, TextureUnit unit) const {  }
void VK13ShaderProgram::setUniform(const ShaderProgramID& sp, int location, ImageUnit unit) const {  }

}