/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vector>

#include <RealEngine/rendering/batches/Circle.hpp>
#include <RealEngine/rendering/Pipeline.hpp>
#include <RealEngine/rendering/buffers/Buffer.hpp>
#include <RealEngine/rendering/vertices/Vertex.hpp>
#include <RealEngine/rendering/DescriptorSet.hpp>

namespace RE {

/**
 * @brief Draws geometric primitives
 * @tparam R The renderer that will perform the commands
*/
template<Renderer R = RendererLateBind>
class GeometryBatch {
public:

    /**
     * @brief Constructs new GeometryBatch
     * @param sources Sources used to construct the shader program that will be used for drawing
    */
    GeometryBatch(const vk::PipelineInputAssemblyStateCreateInfo& ia, const ShaderProgramSources& sources);

    void begin();
    void end();

    void addVertices(uint32_t first, uint32_t count, const RE::VertexPOCO* data, bool separate = true);

    /**
     * @brief Draws the batch with stored shader program
    */
    void draw(const vk::CommandBuffer& commandBuffer, const vk::ArrayProxyNoTemporaries<RE::DescriptorSet<R>>& descriptorSets);

    /**
     * @brief Changes to a different pipeline that will be used for drawing
    */
    void changePipeline(const vk::PipelineInputAssemblyStateCreateInfo& ia, const ShaderProgramSources& sources);

    const Pipeline<R>& getPipeline() const { return m_pipeline; }

private:
    using enum vk::BufferUsageFlagBits;
    using enum vk::MemoryPropertyFlagBits;
    Buffer<R> m_vbo{sizeof(VertexPOCO) * 512, eVertexBuffer, eHostVisible | eHostCoherent};
    Buffer<R> m_ibo{sizeof(uint32_t) * 512, eIndexBuffer, eHostVisible | eHostCoherent};
    VertexPOCO* m_vertices = m_vbo.map<VertexPOCO>(0u, sizeof(VertexPOCO) * 512);
    uint32_t* m_indices = m_ibo.map<uint32_t>(0u, sizeof(uint32_t) * 512);
    uint32_t m_vertexCount = 0u;
    uint32_t m_indexCount = 0u;
    Pipeline<R> m_pipeline;
    vk::PipelineVertexInputStateCreateInfo createVertexInputStateInfo() const;
};

}