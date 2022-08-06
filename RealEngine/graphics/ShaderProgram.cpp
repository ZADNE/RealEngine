/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/ShaderProgram.hpp>

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

void ShaderProgram::dispatchCompute(GLintptr indirect, bool use) const {
	s_impl->dispatchCompute(*this, indirect, use);
}

void ShaderProgram::printInfo() const {
	s_impl->printInfo(*this);
}

void ShaderProgram::backInterfaceBlock(GLuint interfaceBlockIndex, const BufferTypedIndex& index) const {
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

void ShaderProgram::setUniform(int location, GLint val) const { s_impl->setUniform(*this, location, val); }
void ShaderProgram::setUniform(int location, GLint val0, GLint val1) const { s_impl->setUniform(*this, location, val0, val1); }
void ShaderProgram::setUniform(int location, const glm::ivec2& val) const { setUniform(location, val.x, val.y); }
void ShaderProgram::setUniform(int location, GLint val0, GLint val1, GLint val2) const { s_impl->setUniform(*this, location, val0, val1, val2); }
void ShaderProgram::setUniform(int location, const glm::ivec3& val) const { setUniform(location, val.x, val.y, val.z); }
void ShaderProgram::setUniform(int location, GLint val0, GLint val1, GLint val2, GLint val3) const { s_impl->setUniform(*this, location, val0, val1, val2, val3); }
void ShaderProgram::setUniform(int location, const glm::ivec4& val) const { setUniform(location, val.x, val.y, val.z, val.w); }
void ShaderProgram::setUniform(int location, int count, const GLint* val) const { s_impl->setUniform(*this, location, count, val); }
void ShaderProgram::setUniform(int location, int count, const glm::ivec2* val) const { s_impl->setUniform(*this, location, count, val); }
void ShaderProgram::setUniform(int location, int count, const glm::ivec3* val) const { s_impl->setUniform(*this, location, count, val); }
void ShaderProgram::setUniform(int location, int count, const glm::ivec4* val) const { s_impl->setUniform(*this, location, count, val); }

void ShaderProgram::setUniform(int location, GLuint val) const { s_impl->setUniform(*this, location, val); }
void ShaderProgram::setUniform(int location, GLuint val0, GLuint val1) const { s_impl->setUniform(*this, location, val0, val1); }
void ShaderProgram::setUniform(int location, const glm::uvec2& val) const { setUniform(location, val.x, val.y); }
void ShaderProgram::setUniform(int location, GLuint val0, GLuint val1, GLuint val2) const { s_impl->setUniform(*this, location, val0, val1, val2); }
void ShaderProgram::setUniform(int location, const glm::uvec3& val) const { setUniform(location, val.x, val.y, val.z); }
void ShaderProgram::setUniform(int location, GLuint val0, GLuint val1, GLuint val2, GLuint val3) const { s_impl->setUniform(*this, location, val0, val1, val2, val3); }
void ShaderProgram::setUniform(int location, const glm::uvec4& val) const { setUniform(location, val.x, val.y, val.z, val.w); }
void ShaderProgram::setUniform(int location, int count, const GLuint* val) const { s_impl->setUniform(*this, location, count, val); }
void ShaderProgram::setUniform(int location, int count, const glm::uvec2* val) const { s_impl->setUniform(*this, location, count, val); }
void ShaderProgram::setUniform(int location, int count, const glm::uvec3* val) const { s_impl->setUniform(*this, location, count, val); }
void ShaderProgram::setUniform(int location, int count, const glm::uvec4* val) const { s_impl->setUniform(*this, location, count, val); }

void ShaderProgram::setUniform(int location, int count, const glm::mat4* val) const { s_impl->setUniform(*this, location, count, val); }

void ShaderProgram::setUniform(int location, TextureUnit unit) const { s_impl->setUniform(*this, location, unit); }
void ShaderProgram::setUniform(int location, ImageUnit unit) const { s_impl->setUniform(*this, location, unit); }

}