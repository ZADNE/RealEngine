/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/vertices/ShaderProgram.hpp>

#include <RealEngine/rendering/internal_renderers/GL46ShaderProgram.hpp>
#include <RealEngine/rendering/internal_renderers/VK13ShaderProgram.hpp>

namespace RE {

template<Renderer R>
ShaderProgram<R>::ShaderProgram(const ShaderProgramSources& sources):
    m_id(s_impl->construct(sources)) {
}

template<Renderer R>
ShaderProgram<R>::ShaderProgram(ShaderProgram<R>&& other) noexcept :
    m_id(std::move(other.m_id)) {
}

template<Renderer R>
ShaderProgram<R>& ShaderProgram<R>::operator=(ShaderProgram<R>&& other) noexcept {
    m_id = std::move(other.m_id);
    return *this;
}

template<Renderer R>
ShaderProgram<R>::~ShaderProgram() {
    s_impl->destruct(m_id);
}

template<Renderer R>
void ShaderProgram<R>::use() const {
    s_impl->use(m_id);
}

template<Renderer R>
void ShaderProgram<R>::unuse() const {
    s_impl->unuse(m_id);
}

template<Renderer R>
void ShaderProgram<R>::dispatchCompute(const glm::uvec3& groupCount, bool use) const {
    s_impl->dispatchCompute(m_id, groupCount, use);
}

template<Renderer R>
void ShaderProgram<R>::dispatchCompute(int indirect, bool use) const {
    s_impl->dispatchCompute(m_id, indirect, use);
}

template<Renderer R>
void ShaderProgram<R>::printInfo() const {
    s_impl->printInfo(m_id);
}

template<Renderer R>
void ShaderProgram<R>::backInterfaceBlock(unsigned int interfaceBlockIndex, const BufferTypedIndex& index) const {
    s_impl->backInterfaceBlock(m_id, interfaceBlockIndex, index);
}

template<Renderer R>
int ShaderProgram<R>::getUniformLocation(const std::string& name) const {
    return s_impl->getUniformLocation(m_id, name);
}

template<Renderer R> void ShaderProgram<R>::setUniform(int location, float val) const { s_impl->setUniform(m_id, location, val); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, float val0, float val1) const { s_impl->setUniform(m_id, location, val0, val1); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, const glm::vec2& val) const { setUniform(location, val.x, val.y); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, float val0, float val1, float val2) const { s_impl->setUniform(m_id, location, val0, val1, val2); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, const glm::vec3& val) const { setUniform(location, val.x, val.y, val.z); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, float val0, float val1, float val2, float val3) const { s_impl->setUniform(m_id, location, val0, val1, val2, val3); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, const glm::vec4& val) const { setUniform(location, val.x, val.y, val.z, val.w); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, int count, const float* val) const { s_impl->setUniform(m_id, location, count, val); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, int count, const glm::vec2* val) const { s_impl->setUniform(m_id, location, count, val); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, int count, const glm::vec3* val) const { s_impl->setUniform(m_id, location, count, val); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, int count, const glm::vec4* val) const { s_impl->setUniform(m_id, location, count, val); }

template<Renderer R> void ShaderProgram<R>::setUniform(int location, int val) const { s_impl->setUniform(m_id, location, val); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, int val0, int val1) const { s_impl->setUniform(m_id, location, val0, val1); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, const glm::ivec2& val) const { setUniform(location, val.x, val.y); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, int val0, int val1, int val2) const { s_impl->setUniform(m_id, location, val0, val1, val2); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, const glm::ivec3& val) const { setUniform(location, val.x, val.y, val.z); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, int val0, int val1, int val2, int val3) const { s_impl->setUniform(m_id, location, val0, val1, val2, val3); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, const glm::ivec4& val) const { setUniform(location, val.x, val.y, val.z, val.w); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, int count, const int* val) const { s_impl->setUniform(m_id, location, count, val); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, int count, const glm::ivec2* val) const { s_impl->setUniform(m_id, location, count, val); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, int count, const glm::ivec3* val) const { s_impl->setUniform(m_id, location, count, val); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, int count, const glm::ivec4* val) const { s_impl->setUniform(m_id, location, count, val); }

template<Renderer R> void ShaderProgram<R>::setUniform(int location, unsigned int val) const { s_impl->setUniform(m_id, location, val); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, unsigned int val0, unsigned int val1) const { s_impl->setUniform(m_id, location, val0, val1); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, const glm::uvec2& val) const { setUniform(location, val.x, val.y); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, unsigned int val0, unsigned int val1, unsigned int val2) const { s_impl->setUniform(m_id, location, val0, val1, val2); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, const glm::uvec3& val) const { setUniform(location, val.x, val.y, val.z); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, unsigned int val0, unsigned int val1, unsigned int val2, unsigned int val3) const { s_impl->setUniform(m_id, location, val0, val1, val2, val3); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, const glm::uvec4& val) const { setUniform(location, val.x, val.y, val.z, val.w); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, int count, const unsigned int* val) const { s_impl->setUniform(m_id, location, count, val); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, int count, const glm::uvec2* val) const { s_impl->setUniform(m_id, location, count, val); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, int count, const glm::uvec3* val) const { s_impl->setUniform(m_id, location, count, val); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, int count, const glm::uvec4* val) const { s_impl->setUniform(m_id, location, count, val); }

template<Renderer R> void ShaderProgram<R>::setUniform(int location, int count, const glm::mat4* val) const { s_impl->setUniform(m_id, location, count, val); }

template<Renderer R> void ShaderProgram<R>::setUniform(int location, TextureUnit unit) const { s_impl->setUniform(m_id, location, unit); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, ImageUnit unit) const { s_impl->setUniform(m_id, location, unit); }

template class ShaderProgram<RendererLateBind>;
template class ShaderProgram<RendererVK13>;
template class ShaderProgram<RendererGL46>;

}