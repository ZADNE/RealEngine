/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/internal_renderers/VK13Pipeline.hpp>

#include <vulkan/vulkan_raii.hpp>

#include <spirv_glsl.hpp>

namespace RE {

static vk::ShaderStageFlagBits convert(size_t st) {
    using enum ShaderType;
    switch (static_cast<ShaderType>(st)) {
    case VERTEX:            return vk::ShaderStageFlagBits::eVertex;
    case TESS_CONTROL:      return vk::ShaderStageFlagBits::eTessellationControl;
    case TESS_EVALUATION:   return vk::ShaderStageFlagBits::eTessellationEvaluation;
    case GEOMETRY:          return vk::ShaderStageFlagBits::eGeometry;
    case FRAGMENT:          return vk::ShaderStageFlagBits::eFragment;
    case COMPUTE:           return vk::ShaderStageFlagBits::eCompute;
    default: throw Exception{"Unknown shader type"};
    }
}

VK13Pipeline::VK13Pipeline(
    const vk::Device& device, const vk::PipelineCache& pipelineCache,
    const vk::RenderPass& renderPass) :
    m_device(device), m_pipelineCache(pipelineCache),
    m_renderPass(renderPass) {
}

void VK13Pipeline::setCommandBuffer(const vk::CommandBuffer* commandBuffer) {
    m_commandBuffer = commandBuffer;
}

PipelineID VK13Pipeline::construct(const vk::PipelineVertexInputStateCreateInfo& vi, const ShaderProgramSources& srcs) const {
    //Create shader modules
    std::array<vk::ShaderModule, 6> modules;
    std::array<vk::PipelineShaderStageCreateInfo, 6> stages;
    std::vector<vk::DescriptorSetLayoutBinding> dslbs;
    uint32_t shaderCount = 0;
    for (size_t st = 0; st < ShaderProgramSources::NUM_STAGES; ++st) {
        if (!srcs[st].vk13.empty()) {
            modules[shaderCount] = m_device.createShaderModule({
                vk::ShaderModuleCreateFlags{},
                srcs[st].vk13.size() * 4,
                srcs[st].vk13.data()}
            );
            stages[shaderCount] = vk::PipelineShaderStageCreateInfo{
                vk::PipelineShaderStageCreateFlags{},
                convert(st),
                modules[shaderCount],
                "main"
            };
            reflect(srcs[st], convert(st), dslbs);
            shaderCount++;
        }
    }
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{{},
        vk::PrimitiveTopology::eTriangleStrip,
        false                                   //Enable primitive restart
    };
    vk::PipelineViewportStateCreateInfo viewport{{},
        nullptr,                                //Viewport
        nullptr                                 //Scissor
    };
    vk::PipelineRasterizationStateCreateInfo rasterization{
        vk::PipelineRasterizationStateCreateFlags{},
        false,                                  //Enable depth clamping
        false,                                  //Enable rasterizer discard
        vk::PolygonMode::eFill,
        vk::CullModeFlagBits::eNone,
        vk::FrontFace::eClockwise,
        false,                                  //Enable depth bias
        0.0f,                                   //Depth bias constant factor
        0.0f,                                   //Depth bias clamp
        0.0f,                                   //Depth bias slope factor
        1.0f,                                   //Line width
    };
    vk::PipelineMultisampleStateCreateInfo multisample{};
    vk::PipelineColorBlendAttachmentState colorBlendAttachment{false};
    vk::PipelineColorBlendStateCreateInfo colorBlend{{},
        false, vk::LogicOp::eClear,             //Logic op (disable)
        colorBlendAttachment
    };
    std::array dynamicStates = std::to_array<vk::DynamicState>({
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
        });
    vk::PipelineDynamicStateCreateInfo dynamic{{}, dynamicStates};
    vk::DescriptorSetLayout descriptorSetLayout{
        m_device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo{{}, dslbs})
    };
    vk::PipelineLayout pipelineLayout{
        m_device.createPipelineLayout(vk::PipelineLayoutCreateInfo{{},
        descriptorSetLayout})
    };
    //Create pipeline
    vk::GraphicsPipelineCreateInfo createInfo{vk::PipelineCreateFlags{},
        shaderCount, stages.data(),             //Shader modules
        &vi,
        &inputAssembly,
        nullptr,                                //Tesselation
        &viewport,
        &rasterization,
        &multisample,
        nullptr,                                //Depth & stencil
        &colorBlend,
        &dynamic,
        pipelineLayout,
        m_renderPass,
        0u,                                     //Subpass index
        nullptr, -1                             //No base pipeline
    };
    PipelineID::VK13 id{
        .descriptorSetLayout = descriptorSetLayout,
        .pipelineLayout = pipelineLayout,
        .pipeline = m_device.createGraphicsPipeline(m_pipelineCache, createInfo).value
    };
    //Destroy shader modules
    for (uint32_t i = 0; i < shaderCount; i++) {
        m_device.destroyShaderModule(modules[i]);
    }
    return id;
}

void VK13Pipeline::destruct(PipelineID& pl) const {
    m_device.destroyPipeline(pl.m_.vk13.pipeline);
    m_device.destroyPipelineLayout(pl.m_.vk13.pipelineLayout);
    m_device.destroyDescriptorSetLayout(pl.m_.vk13.descriptorSetLayout);
}

void VK13Pipeline::bind(const PipelineID& pl, vk::PipelineBindPoint bindPoint) const {
    m_commandBuffer->bindPipeline(bindPoint, pl.m_.vk13.pipeline);
}

void VK13Pipeline::draw(const PipelineID& pl, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const {
    (void)pl;
    m_commandBuffer->draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

void VK13Pipeline::reflect(const ShaderSourceRef& src, vk::ShaderStageFlagBits st, std::vector<vk::DescriptorSetLayoutBinding>& dslbs) const {
    auto compiler = spirv_cross::Compiler{src.vk13.data(), src.vk13.size()};
    auto resources = compiler.get_shader_resources();
    for (const auto& ub : resources.uniform_buffers) {
        dslbs.emplace_back(
            compiler.get_decoration(ub.id, spv::DecorationBinding), //Binding
            vk::DescriptorType::eUniformBuffer,                     //Type
            1,                                                      //Count
            st                                                      //Stages
        );
    }
    for (const auto& ub : resources.sampled_images) {
        dslbs.emplace_back(
            compiler.get_decoration(ub.id, spv::DecorationBinding), //Binding
            vk::DescriptorType::eCombinedImageSampler,              //Type
            1,                                                      //Count
            st                                                      //Stages
        );
    }
}

}