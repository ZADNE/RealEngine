/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/vertices/ShaderProgram.hpp>

namespace RE {

IShaderProgram* ShaderProgram::s_impl = nullptr;

ShaderProgram::ShaderProgram(const ShaderProgramSources& sources) {
	s_impl->construct(*this, sources);
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept :
	m_ID(other.m_ID) {
	other.m_ID = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
	std::swap(m_ID, other.m_ID);
	return *this;
}

ShaderProgram::~ShaderProgram() {
	s_impl->destruct(*this);
}

void ShaderProgram::use() const {
	s_impl->use(*this);
}

void ShaderProgram::unuse() const {
	s_impl->unuse(*this);
}

void ShaderProgram::dispatchCompute(const glm::uvec3& groupCount, bool use) const {
	s_impl->dispatchCompute(*this, groupCount, use);
}

void ShaderProgram::dispatchCompute(int indirect, bool use) const {
	s_impl->dispatchCompute(*this, indirect, use);
}

void ShaderProgram::printInfo() const {
	s_impl->printInfo(*this);
}

void ShaderProgram::backInterfaceBlock(unsigned int interfaceBlockIndex, const BufferTypedIndex& index) const {
	s_impl->backInterfaceBlock(*this, interfaceBlockIndex, index);
}

void ShaderProgram::setUniform(int location, float val) const { s_impl->setUniform(*this, location, val); }
void ShaderProgram::setUniform(int location, float val0, float val1) const { s_impl->setUniform(*this, location, val0, val1); }
void ShaderProgram::setUniform(int location, const glm::vec2& val) const { setUniform(location, val.x, val.y); }
void ShaderProgram::setUniform(int location, float val0, float val1, float val2) const { s_impl->setUniform(*this, location, val0, val1, val2); }
void ShaderProgram::setUniform(int location, const glm::vec3& val) const { setUniform(location, val.x, val.y, val.z); }
void ShaderProgram::setUniform(int location, float val0, float val1, float val2, float val3) const { s_impl->setUniform(*this, location, val0, val1, val2, val3); }
void ShaderProgram::setUniform(int location, const glm::vec4& val) const { setUniform(location, val.x, val.y, val.z, val.w); }
void ShaderProgram::setUniform(int location, int count, const float* val) const { s_impl->setUniform(*this, location, count, val); }
void ShaderProgram::setUniform(int location, int count, const glm::vec2* val) const { s_impl->setUniform(*this, location, count, val); }
void ShaderProgram::setUniform(int location, int count, const glm::vec3* val) const { s_impl->setUniform(*this, location, count, val); }
void ShaderProgram::setUniform(int location, int count, const glm::vec4* val) const { s_impl->setUniform(*this, location, count, val); }

void ShaderProgram::setUniform(int location, int val) const { s_impl->setUniform(*this, location, val); }
void ShaderProgram::setUniform(int location, int val0, int val1) const { s_impl->setUniform(*this, location, val0, val1); }
void ShaderProgram::setUniform(int location, const glm::ivec2& val) const { setUniform(location, val.x, val.y); }
void ShaderProgram::setUniform(int location, int val0, int val1, int val2) const { s_impl->setUniform(*this, location, val0, val1, val2); }
void ShaderProgram::setUniform(int location, const glm::ivec3& val) const { setUniform(location, val.x, val.y, val.z); }
void ShaderProgram::setUniform(int location, int val0, int val1, int val2, int val3) const { s_impl->setUniform(*this, location, val0, val1, val2, val3); }
void ShaderProgram::setUniform(int location, const glm::ivec4& val) const { setUniform(location, val.x, val.y, val.z, val.w); }
void ShaderProgram::setUniform(int location, int count, const int* val) const { s_impl->setUniform(*this, location, count, val); }
void ShaderProgram::setUniform(int location, int count, const glm::ivec2* val) const { s_impl->setUniform(*this, location, count, val); }
void ShaderProgram::setUniform(int location, int count, const glm::ivec3* val) const { s_impl->setUniform(*this, location, count, val); }
void ShaderProgram::setUniform(int location, int count, const glm::ivec4* val) const { s_impl->setUniform(*this, location, count, val); }

void ShaderProgram::setUniform(int location, unsigned int val) const { s_impl->setUniform(*this, location, val); }
void ShaderProgram::setUniform(int location, unsigned int val0, unsigned int val1) const { s_impl->setUniform(*this, location, val0, val1); }
void ShaderProgram::setUniform(int location, const glm::uvec2& val) const { setUniform(location, val.x, val.y); }
void ShaderProgram::setUniform(int location, unsigned int val0, unsigned int val1, unsigned int val2) const { s_impl->setUniform(*this, location, val0, val1, val2); }
void ShaderProgram::setUniform(int location, const glm::uvec3& val) const { setUniform(location, val.x, val.y, val.z); }
void ShaderProgram::setUniform(int location, unsigned int val0, unsigned int val1, unsigned int val2, unsigned int val3) const { s_impl->setUniform(*this, location, val0, val1, val2, val3); }
void ShaderProgram::setUniform(int location, const glm::uvec4& val) const { setUniform(location, val.x, val.y, val.z, val.w); }
void ShaderProgram::setUniform(int location, int count, const unsigned int* val) const { s_impl->setUniform(*this, location, count, val); }
void ShaderProgram::setUniform(int location, int count, const glm::uvec2* val) const { s_impl->setUniform(*this, location, count, val); }
void ShaderProgram::setUniform(int location, int count, const glm::uvec3* val) const { s_impl->setUniform(*this, location, count, val); }
void ShaderProgram::setUniform(int location, int count, const glm::uvec4* val) const { s_impl->setUniform(*this, location, count, val); }

void ShaderProgram::setUniform(int location, int count, const glm::mat4* val) const { s_impl->setUniform(*this, location, count, val); }

void ShaderProgram::setUniform(int location, TextureUnit unit) const { s_impl->setUniform(*this, location, unit); }
void ShaderProgram::setUniform(int location, ImageUnit unit) const { s_impl->setUniform(*this, location, unit); }

}