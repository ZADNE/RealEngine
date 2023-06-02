/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <glm/mat4x4.hpp>

#include <RealEngine/graphics/buffers/BufferMapped.hpp>
#include <RealEngine/graphics/pipelines/Pipeline.hpp>
#include <RealEngine/graphics/pipelines/PipelineLayout.hpp>
#include <RealEngine/graphics/pipelines/Vertex.hpp>

namespace re {

/**
 * @brief Draws geometric primitives
 */
class GeometryBatch {
public:
    /**
     * @brief Constructs GeometryBatch
     * @param topology The topology that the batch will draw
     * @param maxVertices Maximum number of vertices that can be in the batch
     * @param lineWidth Width of the lines, in pixels
     * (relevant only if the topology is a part of the line class)
     */
    GeometryBatch(
        vk::PrimitiveTopology topology,
        unsigned int          maxVertices,
        float                 lineWidth
    );

    /**
     * @brief Begins new batch
     * @detail All vertices have to be added between begin() and end()
     */
    void begin();

    /**
     * @brief Ends the batch
     * @detail All vertices have to be added between begin() and end()
     */
    void end();

    /**
     * @brief Adds vertices that will be rendered
     * @detail Must be called between begin() and end().
     */
    void addVertices(uint32_t first, uint32_t count, const VertexPOCO* data);

    template<size_t size>
    void addVertices(const std::array<VertexPOCO, size>& vertices) {
        addVertices(0u, size, vertices.data());
    }

    /**
     * @brief Draws the batch
     * @detail The whole geometry is drawn in the order it was added in
     * @param commandBuffer Command buffer used for rendering
     * @param mvpMat Transformation matrix applied to the sprites
     */
    void draw(const vk::CommandBuffer& commandBuffer, const glm::mat4& mvpMat);

private:
    BufferMapped<VertexPOCO>               m_verticesBuf;
    uint32_t                               m_nextVertexIndex;
    uint32_t                               m_maxVertices;
    PipelineLayout                         m_pipelineLayout;
    Pipeline                               m_pipeline;
    vk::PipelineVertexInputStateCreateInfo createVertexInputStateInfo() const;
};

} // namespace re