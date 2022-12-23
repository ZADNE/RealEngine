/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/batches/GeometryBatch.hpp>

#include <RealEngine/rendering/PerFrameInFlight.hpp>
#include <RealEngine/rendering/batches/shaders/AllShaders.hpp>

namespace RE {

using enum vk::BufferUsageFlagBits;
using enum vk::MemoryPropertyFlagBits;

GeometryBatch::GeometryBatch(vk::PrimitiveTopology topology, unsigned int maxVertices) :
    m_verticesBuf(sizeof(VertexPOCO)* maxVertices* MAX_FRAMES_IN_FLIGHT, eVertexBuffer, eHostVisible | eHostCoherent),
    m_verticesMapped(m_verticesBuf.map<VertexPOCO>(0, sizeof(VertexPOCO)* maxVertices* MAX_FRAMES_IN_FLIGHT)),
    m_maxVertices(maxVertices),
    m_pipeline(
        PipelineCreateInfo{
            .vertexInput = createVertexInputStateInfo(),
            .topology = topology
        },
        PipelineSources{
            .vert = geometry_vert,
            .frag = geometry_frag
        }
    ) {
}

void GeometryBatch::begin() {
    m_nextVertexIndex = m_maxVertices * NEXT_FRAME;
}

void GeometryBatch::end() {
    //Nothing to do :-)
}

void GeometryBatch::addVertices(uint32_t first, uint32_t countVer, const VertexPOCO* data) {
    std::memcpy(&m_verticesMapped[m_nextVertexIndex], &data[first], sizeof(VertexPOCO) * countVer);
    m_nextVertexIndex += countVer;
}

void GeometryBatch::draw(const vk::CommandBuffer& commandBuffer, const glm::mat4& mvpMat) {
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *m_pipeline);
    commandBuffer.bindVertexBuffers(0u, *m_verticesBuf, 0ull);
    commandBuffer.pushConstants<glm::mat4>(m_pipeline.pipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0u, mvpMat);
    commandBuffer.draw(m_nextVertexIndex - m_maxVertices * NEXT_FRAME, 1u, 0u, 0u);
}

vk::PipelineVertexInputStateCreateInfo GeometryBatch::createVertexInputStateInfo() const {
    static constexpr std::array bindings = std::to_array<vk::VertexInputBindingDescription>({{
        0u,                             //Binding index
        sizeof(VertexPOCO),             //Stride
        vk::VertexInputRate::eVertex    //Input rate
    }});
    static constexpr std::array attributes = std::to_array<vk::VertexInputAttributeDescription>({{
        ATTR_POSITION,                  //Location
        0u,                             //Binding index
        vk::Format::eR32G32B32A32Sfloat,//Format
        offsetof(VertexPOCO, position)  //Relative offset
    },{
        ATTR_COLOR,                     //Location
        0u,                             //Binding index
        vk::Format::eR8G8B8A8Unorm,     //Format
        offsetof(VertexPOCO, color)     //Relative offset
    }});
    return vk::PipelineVertexInputStateCreateInfo{{}, bindings, attributes};
}

}