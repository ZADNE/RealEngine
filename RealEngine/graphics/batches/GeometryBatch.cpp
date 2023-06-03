/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/PerFrameInFlight.hpp>
#include <RealEngine/graphics/batches/GeometryBatch.hpp>
#include <RealEngine/graphics/batches/shaders/AllShaders.hpp>

namespace re {

using enum vk::BufferUsageFlagBits;
using enum vk::MemoryPropertyFlagBits;
using enum vma::AllocationCreateFlagBits;

GeometryBatch::GeometryBatch(
    vk::PrimitiveTopology topology, unsigned int maxVertices, float lineWidth
)
    : m_verticesBuf(BufferCreateInfo{
          .allocFlags  = eMapped | eHostAccessSequentialWrite,
          .sizeInBytes = sizeof(VertexPOCO) * maxVertices * k_maxFramesInFlight,
          .usage       = eVertexBuffer})
    , m_maxVertices(maxVertices)
    , m_pipelineLayout(
          PipelineLayoutCreateInfo{},
          PipelineGraphicsSources{.vert = geometry_vert, .frag = geometry_frag}
      )
    , m_pipeline(
          PipelineGraphicsCreateInfo{
              .pipelineLayout = *m_pipelineLayout,
              .vertexInput    = createVertexInputStateInfo(),
              .topology       = topology,
              .lineWidth      = lineWidth},
          PipelineGraphicsSources{.vert = geometry_vert, .frag = geometry_frag}
      ) {
}

void GeometryBatch::begin() {
    m_nextVertexIndex = m_maxVertices * g_nextFrame;
}

void GeometryBatch::end() {
    // Nothing to do :-)
}

void GeometryBatch::addVertices(
    uint32_t first, uint32_t countVer, const VertexPOCO* data
) {
    std::memcpy(
        &m_verticesBuf[m_nextVertexIndex], &data[first], sizeof(VertexPOCO) * countVer
    );
    m_nextVertexIndex += countVer;
}

void GeometryBatch::draw(const vk::CommandBuffer& commandBuffer, const glm::mat4& mvpMat) {
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *m_pipeline);
    commandBuffer.bindVertexBuffers(0u, m_verticesBuf.buffer(), 0ull);
    commandBuffer.pushConstants<glm::mat4>(
        *m_pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0u, mvpMat
    );
    commandBuffer.draw(
        m_nextVertexIndex - m_maxVertices * g_nextFrame,
        1u,
        m_maxVertices * g_nextFrame,
        0u
    );
}

vk::PipelineVertexInputStateCreateInfo GeometryBatch::createVertexInputStateInfo() const {
    static constexpr std::array bindings =
        std::to_array<vk::VertexInputBindingDescription>({{
            0u,                          // Binding index
            sizeof(VertexPOCO),          // Stride
            vk::VertexInputRate::eVertex // Input rate
        }});
    static constexpr std::array attributes =
        std::to_array<vk::VertexInputAttributeDescription>(
            {{
                 0u,                              // Location
                 0u,                              // Binding index
                 vk::Format::eR32G32B32A32Sfloat, // Format
                 offsetof(VertexPOCO, position)   // Relative offset
             },
             {
                 1u,                         // Location
                 0u,                         // Binding index
                 vk::Format::eR8G8B8A8Unorm, // Format
                 offsetof(VertexPOCO, color) // Relative offset
             }}
        );
    return vk::PipelineVertexInputStateCreateInfo{{}, bindings, attributes};
}

} // namespace re