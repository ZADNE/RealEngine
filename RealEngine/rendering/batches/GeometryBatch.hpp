/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <glm/mat4x4.hpp>

#include <RealEngine/rendering/pipelines/Pipeline.hpp>
#include <RealEngine/rendering/buffers/Buffer.hpp>
#include <RealEngine/rendering/pipelines/Vertex.hpp>

namespace RE {

/**
 * @brief Draws geometric primitives
*/
class GeometryBatch {
public:

    /**
     * @brief Constructs GeometryBatch
     * @param topology The topology that the batch will draw
     * @param maxVertices Maximum number of vertices that can be in the batch
    */
    GeometryBatch(vk::PrimitiveTopology topology, unsigned int maxVertices);

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

    void addVertices(uint32_t first, uint32_t count, const VertexPOCO* data);

    /**
     * @brief Draws the batch
     * @detail The whole geometry is drawn in the order it was added in
     * @param commandBuffer Command buffer used for rendering
     * @param mvpMat Transformation matrix applied to the sprites
    */
    void draw(const vk::CommandBuffer& commandBuffer, const glm::mat4& mvpMat);

private:

    Buffer m_verticesBuf;
    VertexPOCO* m_verticesMapped = nullptr;
    uint32_t m_nextVertexIndex;
    uint32_t m_maxVertices;
    Pipeline m_pipeline;
    vk::PipelineVertexInputStateCreateInfo createVertexInputStateInfo() const;
};

}