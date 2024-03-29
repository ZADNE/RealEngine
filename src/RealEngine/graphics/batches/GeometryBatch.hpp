﻿/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <span>

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
        vk::PrimitiveTopology topology, unsigned int maxVertices, float lineWidth = 1.0f
    );

    /**
     * @brief   Begins new batch
     * @details All vertices have to be added between begin() and end()
     */
    void begin();

    /**
     * @brief   Ends the batch
     * @details All vertices have to be added between begin() and end()
     */
    void end();

    /**
     * @brief   Adds vertices that will be rendered
     * @details Must be called between begin() and end().
     */
    void addVertices(std::span<const VertexPoCo> vertices);

    /**
     * @brief   Draws the batch
     * @details The whole geometry is drawn in the order it was added in
     * @param cmdBuf Command buffer used for rendering
     * @param mvpMat Transformation matrix applied to the sprites
     */
    void draw(const CommandBuffer& cmdBuf, const glm::mat4& mvpMat);

private:
    BufferMapped<VertexPoCo> m_verticesBuf;
    uint32_t                 m_nextVertexIndex;
    uint32_t                 m_maxVertices;
    PipelineLayout           m_pipelineLayout;
    Pipeline                 m_pipeline;
};

} // namespace re