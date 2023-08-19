﻿/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/batches/GeometryBatch.hpp>
#include <RealEngine/graphics/batches/shaders/AllShaders.hpp>
#include <RealEngine/graphics/synchronization/DoubleBuffered.hpp>

namespace re {

using enum vk::BufferUsageFlagBits;
using enum vk::MemoryPropertyFlagBits;
using enum vma::AllocationCreateFlagBits;

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
              .pipelineLayout = *m_pipelineLayout,
              .vertexInput    = createVertexInputStateInfo(),
              .topology       = topology,
              .lineWidth      = lineWidth},
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

void GeometryBatch::draw(const vk::CommandBuffer& commandBuffer, const glm::mat4& mvpMat) {
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *m_pipeline);
    commandBuffer.bindVertexBuffers(0u, m_verticesBuf.buffer(), vk::DeviceSize{0});
    commandBuffer.pushConstants<glm::mat4>(
        *m_pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0u, mvpMat
    );
    commandBuffer.draw(
        m_nextVertexIndex -
            m_maxVertices * FrameDoubleBufferingState::writeIndex(),
        1u,
        m_maxVertices * FrameDoubleBufferingState::writeIndex(),
        0u
    );
}

vk::PipelineVertexInputStateCreateInfo GeometryBatch::createVertexInputStateInfo() const {
    static constexpr std::array bindings =
        std::to_array<vk::VertexInputBindingDescription>({{
            0u,                          // Binding index
            sizeof(VertexPoCo),          // Stride
            vk::VertexInputRate::eVertex // Input rate
        }});
    static constexpr std::array attributes =
        std::to_array<vk::VertexInputAttributeDescription>(
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
    return vk::PipelineVertexInputStateCreateInfo{{}, bindings, attributes};
}

} // namespace re