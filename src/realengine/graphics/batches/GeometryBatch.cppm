/**
 *  @author    Dubsky Tomas
 */
export module realengine:graphics.batches.GeometryBatch;
import std;
import glm;
import :graphics.buffers.BufferMapped;
import :graphics.pipelines.Pipeline;
import :graphics.pipelines.PipelineLayout;
import :graphics.pipelines.Vertex;

export namespace re {

struct GeometryBatchCreateInfo {
    /**
     * @brief The topology that the batch will draw
     */
    vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList;
    /**
     * @brief The renderpass that the batch will always draw in
     */
    RenderPassSubpass renderPassSubpass{};
    /**
     * @brief Is the maximum number of vertices that can be in the batch
     */
    unsigned int maxVertices = 0u;
    /**
     * @brief Width of the lines, in pixels
     * (relevant only if the topology is a part of the line class)
     */
    float lineWidthPx = 1.0f;
};

/**
 * @brief Draws geometric primitives (points, lines, triangles)
 */
class GeometryBatch {
public:
    /**
     * @brief Constructs a GeometryBatch
     */
    explicit GeometryBatch(const GeometryBatchCreateInfo& createInfo);

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
     * @param cb Command buffer used for rendering
     * @param mvpMat Transformation matrix applied to the sprites
     */
    void draw(const CommandBuffer& cb, const glm::mat4& mvpMat);

private:
    BufferMapped<VertexPoCo> m_verticesBuf;
    uint32_t m_nextVertexIndex{};
    uint32_t m_maxVertices;
    PipelineLayout m_pipelineLayout;
    Pipeline m_pipeline;
};

} // namespace re
