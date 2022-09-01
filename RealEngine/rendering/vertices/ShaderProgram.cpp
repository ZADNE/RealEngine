/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/vertices/ShaderProgram.hpp>

#include <RealEngine/rendering/RendererLateBind.hpp>
#include <RealEngine/rendering/RendererGL46.hpp>

namespace RE {

template<typename R>
ShaderProgram<R>::ShaderProgram(const ShaderProgramSources& sources):
    m_internals(s_impl->construct(sources)) {
}

template<typename R>
ShaderProgram<R>::ShaderProgram(ShaderProgram<R>&& other) noexcept :
    m_internals(std::move(other.m_internals)) {
}

template<typename R>
ShaderProgram<R>& ShaderProgram<R>::operator=(ShaderProgram<R>&& other) noexcept {
    m_internals = std::move(other.m_internals);
    return *this;
}

template<typename R>
ShaderProgram<R>::~ShaderProgram() {
    s_impl->destruct(m_internals);
}

template<typename R>
void ShaderProgram<R>::use() const {
    s_impl->use(m_internals);
}

template<typename R>
void ShaderProgram<R>::unuse() const {
    s_impl->unuse(m_internals);
}

template<typename R>
void ShaderProgram<R>::dispatchCompute(const glm::uvec3& groupCount, bool use) const {
    s_impl->dispatchCompute(m_internals, groupCount, use);
}

template<typename R>
void ShaderProgram<R>::dispatchCompute(int indirect, bool use) const {
    s_impl->dispatchCompute(m_internals, indirect, use);
}

template<typename R>
void ShaderProgram<R>::printInfo() const {
    s_impl->printInfo(m_internals);
}

template<typename R>
void ShaderProgram<R>::backInterfaceBlock(unsigned int interfaceBlockIndex, const BufferTypedIndex& index) const {
    s_impl->backInterfaceBlock(m_internals, interfaceBlockIndex, index);
}

template<typename R> void ShaderProgram<R>::setUniform(int location, float val) const { s_impl->setUniform(m_internals, location, val); }
template<typename R> void ShaderProgram<R>::setUniform(int location, float val0, float val1) const { s_impl->setUniform(m_internals, location, val0, val1); }
template<typename R> void ShaderProgram<R>::setUniform(int location, const glm::vec2& val) const { setUniform(location, val.x, val.y); }
template<typename R> void ShaderProgram<R>::setUniform(int location, float val0, float val1, float val2) const { s_impl->setUniform(m_internals, location, val0, val1, val2); }
template<typename R> void ShaderProgram<R>::setUniform(int location, const glm::vec3& val) const { setUniform(location, val.x, val.y, val.z); }
template<typename R> void ShaderProgram<R>::setUniform(int location, float val0, float val1, float val2, float val3) const { s_impl->setUniform(m_internals, location, val0, val1, val2, val3); }
template<typename R> void ShaderProgram<R>::setUniform(int location, const glm::vec4& val) const { setUniform(location, val.x, val.y, val.z, val.w); }
template<typename R> void ShaderProgram<R>::setUniform(int location, int count, const float* val) const { s_impl->setUniform(m_internals, location, count, val); }
template<typename R> void ShaderProgram<R>::setUniform(int location, int count, const glm::vec2* val) const { s_impl->setUniform(m_internals, location, count, val); }
template<typename R> void ShaderProgram<R>::setUniform(int location, int count, const glm::vec3* val) const { s_impl->setUniform(m_internals, location, count, val); }
template<typename R> void ShaderProgram<R>::setUniform(int location, int count, const glm::vec4* val) const { s_impl->setUniform(m_internals, location, count, val); }

template<typename R> void ShaderProgram<R>::setUniform(int location, int val) const { s_impl->setUniform(m_internals, location, val); }
template<typename R> void ShaderProgram<R>::setUniform(int location, int val0, int val1) const { s_impl->setUniform(m_internals, location, val0, val1); }
template<typename R> void ShaderProgram<R>::setUniform(int location, const glm::ivec2& val) const { setUniform(location, val.x, val.y); }
template<typename R> void ShaderProgram<R>::setUniform(int location, int val0, int val1, int val2) const { s_impl->setUniform(m_internals, location, val0, val1, val2); }
template<typename R> void ShaderProgram<R>::setUniform(int location, const glm::ivec3& val) const { setUniform(location, val.x, val.y, val.z); }
template<typename R> void ShaderProgram<R>::setUniform(int location, int val0, int val1, int val2, int val3) const { s_impl->setUniform(m_internals, location, val0, val1, val2, val3); }
template<typename R> void ShaderProgram<R>::setUniform(int location, const glm::ivec4& val) const { setUniform(location, val.x, val.y, val.z, val.w); }
template<typename R> void ShaderProgram<R>::setUniform(int location, int count, const int* val) const { s_impl->setUniform(m_internals, location, count, val); }
template<typename R> void ShaderProgram<R>::setUniform(int location, int count, const glm::ivec2* val) const { s_impl->setUniform(m_internals, location, count, val); }
template<typename R> void ShaderProgram<R>::setUniform(int location, int count, const glm::ivec3* val) const { s_impl->setUniform(m_internals, location, count, val); }
template<typename R> void ShaderProgram<R>::setUniform(int location, int count, const glm::ivec4* val) const { s_impl->setUniform(m_internals, location, count, val); }

template<typename R> void ShaderProgram<R>::setUniform(int location, unsigned int val) const { s_impl->setUniform(m_internals, location, val); }
template<typename R> void ShaderProgram<R>::setUniform(int location, unsigned int val0, unsigned int val1) const { s_impl->setUniform(m_internals, location, val0, val1); }
template<typename R> void ShaderProgram<R>::setUniform(int location, const glm::uvec2& val) const { setUniform(location, val.x, val.y); }
template<typename R> void ShaderProgram<R>::setUniform(int location, unsigned int val0, unsigned int val1, unsigned int val2) const { s_impl->setUniform(m_internals, location, val0, val1, val2); }
template<typename R> void ShaderProgram<R>::setUniform(int location, const glm::uvec3& val) const { setUniform(location, val.x, val.y, val.z); }
template<typename R> void ShaderProgram<R>::setUniform(int location, unsigned int val0, unsigned int val1, unsigned int val2, unsigned int val3) const { s_impl->setUniform(m_internals, location, val0, val1, val2, val3); }
template<typename R> void ShaderProgram<R>::setUniform(int location, const glm::uvec4& val) const { setUniform(location, val.x, val.y, val.z, val.w); }
template<typename R> void ShaderProgram<R>::setUniform(int location, int count, const unsigned int* val) const { s_impl->setUniform(m_internals, location, count, val); }
template<typename R> void ShaderProgram<R>::setUniform(int location, int count, const glm::uvec2* val) const { s_impl->setUniform(m_internals, location, count, val); }
template<typename R> void ShaderProgram<R>::setUniform(int location, int count, const glm::uvec3* val) const { s_impl->setUniform(m_internals, location, count, val); }
template<typename R> void ShaderProgram<R>::setUniform(int location, int count, const glm::uvec4* val) const { s_impl->setUniform(m_internals, location, count, val); }

template<typename R> void ShaderProgram<R>::setUniform(int location, int count, const glm::mat4* val) const { s_impl->setUniform(m_internals, location, count, val); }

template<typename R> void ShaderProgram<R>::setUniform(int location, TextureUnit unit) const { s_impl->setUniform(m_internals, location, unit); }
template<typename R> void ShaderProgram<R>::setUniform(int location, ImageUnit unit) const { s_impl->setUniform(m_internals, location, unit); }

template ShaderProgram<RendererLateBind>;
template ShaderProgram<RendererGL46>;

}