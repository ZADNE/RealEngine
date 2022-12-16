/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/batches/GeometryBatch.hpp>

#include <cmath>

#include <glm/gtc/constants.hpp>

namespace RE {

GeometryBatch::GeometryBatch(const vk::PipelineInputAssemblyStateCreateInfo& ia, const PipelineSources& sources) :
    m_pipeline(createVertexInputStateInfo(), ia, sources) {
}

void GeometryBatch::begin() {
    m_vertexCount = 0u;
    m_indexCount = 0u;
}

void GeometryBatch::end() {
    //Nothing to do :-)
}

void GeometryBatch::addVertices(uint32_t first, uint32_t countVer, const VertexPOCO* data, bool separate/* = true*/) {
    //Vertices
    std::copy(&data[first], &data[first + countVer], &m_vertices[m_vertexCount]);
    //Indices
    for (uint32_t i = 0u; i < countVer; ++i) {
        m_indices[m_indexCount++] = m_vertexCount++;
    }
    //Separator
    if (separate) {
        m_indices[m_indexCount++] = PRIMITIVE_RESTART_INDEX<uint32_t>();
    }
}

void GeometryBatch::draw(const vk::CommandBuffer& commandBuffer, const vk::ArrayProxyNoTemporaries<DescriptorSet>& descriptorSets) {
    m_pipeline.bind(vk::PipelineBindPoint::eGraphics);
    m_vbo.bindAsVertexBuffer(0u, 0ull);
    m_ibo.bindAsIndexBuffer(0ull, vk::IndexType::eUint32);
    for (const auto& set : descriptorSets) {
        set.bind(vk::PipelineBindPoint::eGraphics, m_pipeline);
    }
    m_pipeline.drawIndexed(m_indexCount, 1u, 0u, 0, 0u);
}

void GeometryBatch::changePipeline(const vk::PipelineInputAssemblyStateCreateInfo& ia, const PipelineSources& sources) {
    m_pipeline = Pipeline{createVertexInputStateInfo(), ia, sources};
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