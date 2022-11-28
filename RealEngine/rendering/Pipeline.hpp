/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IPipeline.hpp>
#include <RealEngine/rendering/Renderer.hpp>

namespace RE {

/**
 * @brief Controls how vertices are rendered to screen.
 * @tparam R The renderer that will perform the commands
*/
template<Renderer R = RendererLateBind>
class Pipeline {
    friend class GL46Fixture; friend class VK13Fixture;
public:

    Pipeline(const vk::PipelineVertexInputStateCreateInfo& vi, const ShaderProgramSources& srcs);

    Pipeline(const Pipeline<R>&) = delete;
    Pipeline(Pipeline<R>&& other) noexcept;

    Pipeline<R>& operator=(const Pipeline<R>&) = delete;
    Pipeline<R>& operator=(Pipeline<R>&& other) noexcept;

    ~Pipeline();

    void bind(vk::PipelineBindPoint bindPoint) const;
    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const;

private:

    PipelineID m_id;

    static inline R::Pipeline* s_impl = nullptr;
};

}
