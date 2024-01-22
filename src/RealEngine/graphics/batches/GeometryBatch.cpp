/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/batches/GeometryBatch.hpp>
#include <RealEngine/graphics/batches/shaders/AllShaders.hpp>
#include <RealEngine/graphics/commands/CommandBuffer.hpp>
#include <RealEngine/graphics/synchronization/DoubleBuffered.hpp>

using enum vk::BufferUsageFlagBits;
using enum vk::MemoryPropertyFlagBits;
using enum vma::AllocationCreateFlagBits;

namespace re {

namespace {
constexpr std::array k_bindings = std::to_array<vk::VertexInputBindingDescription>({{
    0u,                          // Binding index
    sizeof(VertexPoCo),          // Stride
    vk::VertexInputRate::eVertex // Input rate
}});
constexpr std::array k_attributes = std::to_array<vk::VertexInputAttributeDescription>(
    {{
         0u,                              // Location
         0u,                              // Binding index
         vk::Format::eR32G32B32A32Sfloat, // Format
         offsetof(VertexPoCo, position)   // Relative offset
     },
     {
         1u,                         // Location
         0u,                         // Binding index
         vk::Format::eR8G8B8A8Unorm, // Format
         offsetof(VertexPoCo, color) // Relative offset
     }}
);
vk::PipelineVertexInputStateCreateInfo k_vertexInput{{}, k_bindings, k_attributes};
} // namespace

GeometryBatch::GeometryBatch(
    vk::PrimitiveTopology topology, unsigned int maxVertices, float lineWidth
)
    : m_verticesBuf(BufferCreateInfo{
          .allocFlags  = eMapped | eHostAccessSequentialWrite,
          .sizeInBytes = sizeof(VertexPoCo) * maxVertices * k_maxFramesInFlight,
          .usage       = eVertexBuffer})
    , m_maxVertices(maxVertices)
    , m_pipelineLayout(
          PipelineLayoutCreateInfo{},
          PipelineGraphicsSources{.vert = geometry_vert, .frag = geometry_frag}
      )
    , m_pipeline(
          PipelineGraphicsCreateInfo{
              .vertexInput    = &k_vertexInput,
              .topology       = topology,
              .lineWidth      = lineWidth,
              .pipelineLayout = *m_pipelineLayout},
          PipelineGraphicsSources{.vert = geometry_vert, .frag = geometry_frag}
      ) {
}

void GeometryBatch::begin() {
    m_nextVertexIndex = m_maxVertices * FrameDoubleBufferingState::writeIndex();
}

void GeometryBatch::end() {
    // Nothing to do :-)
}

void GeometryBatch::addVertices(std::span<const VertexPoCo> vertices) {
    std::memcpy(
        &m_verticesBuf[m_nextVertexIndex],
        vertices.data(),
        sizeof(VertexPoCo) * vertices.size()
    );
    m_nextVertexIndex += vertices.size();
}

void GeometryBatch::draw(const CommandBuffer& cmdBuf, const glm::mat4& mvpMat) {
    cmdBuf->bindPipeline(vk::PipelineBindPoint::eGraphics, *m_pipeline);
    cmdBuf->bindVertexBuffers(0u, m_verticesBuf.buffer(), vk::DeviceSize{0});
    cmdBuf->pushConstants<glm::mat4>(
        *m_pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0u, mvpMat
    );
    cmdBuf->draw(
        m_nextVertexIndex - m_maxVertices * FrameDoubleBufferingState::writeIndex(),
        1u,
        m_maxVertices * FrameDoubleBufferingState::writeIndex(),
        0u
    );
}

} // namespace re
