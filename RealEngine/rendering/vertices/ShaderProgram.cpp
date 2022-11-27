/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/vertices/ShaderProgram.hpp>

#include <RealEngine/rendering/internal_renderers/VK13ShaderProgram.hpp>

namespace RE {

template<Renderer R>
ShaderProgram<R>::ShaderProgram(const ShaderProgramSources& sources) :
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

template<Renderer R> void ShaderProgram<R>::setUniform(int location, TextureUnit unit) const { s_impl->setUniform(m_id, location, unit); }
template<Renderer R> void ShaderProgram<R>::setUniform(int location, ImageUnit unit) const { s_impl->setUniform(m_id, location, unit); }

template class ShaderProgram<RendererLateBind>;
template class ShaderProgram<RendererVK13>;

}