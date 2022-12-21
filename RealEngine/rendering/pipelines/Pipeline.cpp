/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/pipelines/Pipeline.hpp>

#include <spirv_glsl.hpp>

namespace RE {

static vk::ShaderStageFlagBits convert(size_t st) {
    switch (st) {
    case 0: return vk::ShaderStageFlagBits::eVertex;
    case 1: return vk::ShaderStageFlagBits::eTessellationControl;
    case 2: return vk::ShaderStageFlagBits::eTessellationEvaluation;
    case 3: return vk::ShaderStageFlagBits::eGeometry;
    case 4: return vk::ShaderStageFlagBits::eFragment;
    case 5: return vk::ShaderStageFlagBits::eCompute;
    default: throw Exception{"Unknown shader type"};
    }
}

Pipeline::Pipeline(const vk::PipelineVertexInputStateCreateInfo& vi, const vk::PipelineInputAssemblyStateCreateInfo& ia, const PipelineSources& srcs, uint32_t patchControlPoints/* = 0*/) {
    //Create shader modules
    std::array<vk::ShaderModule, PipelineSources::NUM_STAGES> modules;
    std::array<vk::PipelineShaderStageCreateInfo, PipelineSources::NUM_STAGES> stages;
    std::vector<vk::DescriptorSetLayoutBinding> dslbs;
    std::vector<vk::PushConstantRange> ranges;
    uint32_t shaderCount = 0;
    for (size_t st = 0; st < PipelineSources::NUM_STAGES; ++st) {
        if (!srcs[st].vk13.empty()) {
            modules[shaderCount] = s_device->createShaderModule({{},
                srcs[st].vk13.size() * 4,
                srcs[st].vk13.data()}
            );
            stages[shaderCount] = vk::PipelineShaderStageCreateInfo{{},
                convert(st),
                modules[shaderCount],
                "main"
            };
            reflect(srcs[st], convert(st), dslbs, ranges);
            shaderCount++;
        }
    }
    vk::PipelineTessellationStateCreateInfo tessellation{{},
        patchControlPoints,
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
    using enum vk::BlendFactor;
    using enum vk::BlendOp;
    using enum vk::ColorComponentFlagBits;
    vk::PipelineColorBlendAttachmentState colorBlendAttachment{
        true,                                   //Enable blending
        eSrcAlpha,                              //Src color
        eOneMinusSrcAlpha,                      //Dst color
        eAdd,                                   //Color operation
        eOne,                                   //Src alpha
        eOneMinusSrcAlpha,                      //Dst alpha
        eAdd,                                   //Alpha operation
        eR | eG | eB | eA                       //Write mask
    };
    vk::PipelineColorBlendStateCreateInfo colorBlend{{},
        false, vk::LogicOp::eClear,             //Logic op (disable)
        colorBlendAttachment
    };
    std::array dynamicStates = std::to_array<vk::DynamicState>({
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
    });
    vk::PipelineDynamicStateCreateInfo dynamic{{}, dynamicStates};
    //Create descriptor set
    m_descriptorSetLayout = s_device->createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo{{}, dslbs});
    //Create pipeline layout
    m_pipelineLayout = s_device->createPipelineLayout(vk::PipelineLayoutCreateInfo{{}, m_descriptorSetLayout, ranges});
    //Create pipeline
    vk::GraphicsPipelineCreateInfo createInfo{vk::PipelineCreateFlags{},
        shaderCount, stages.data(),             //Shader modules
        &vi,
        &ia,
        &tessellation,                          //Tesselation
        &viewport,
        &rasterization,
        &multisample,
        nullptr,                                //Depth & stencil
        &colorBlend,
        &dynamic,
        m_pipelineLayout,
        *s_renderPass,
        0u,                                     //Subpass index
        nullptr, -1                             //No base pipeline
    };
    m_pipeline = s_device->createGraphicsPipeline(*s_pipelineCache, createInfo).value;
    //Destroy shader modules
    for (uint32_t i = 0; i < shaderCount; i++) {
        s_device->destroyShaderModule(modules[i]);
    }
}

Pipeline::Pipeline(const ShaderSourceRef& compute) {
    //Create compute shader module
    vk::ShaderModule compShader = s_device->createShaderModule(
        vk::ShaderModuleCreateInfo{{},
            compute.vk13.size() * 4,
            compute.vk13.data()
        }
    );
    std::vector<vk::DescriptorSetLayoutBinding> dslbs;
    std::vector<vk::PushConstantRange> ranges;
    reflect(compute, vk::ShaderStageFlagBits::eCompute, dslbs, ranges);
    //Create descriptor set
    m_descriptorSetLayout = s_device->createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo{{}, dslbs});
    //Create pipeline layout
    m_pipelineLayout = s_device->createPipelineLayout(vk::PipelineLayoutCreateInfo{{}, m_descriptorSetLayout, ranges});
    //Create pipeline
    vk::ComputePipelineCreateInfo createInfo{{},
        vk::PipelineShaderStageCreateInfo{{},
            vk::ShaderStageFlagBits::eCompute,
            compShader,
            "main"
        },
        m_pipelineLayout,
        nullptr, -1                             //No base pipeline
    };
    m_pipeline = s_device->createComputePipeline(*s_pipelineCache, createInfo).value;
    //Destroy compute shader module
    s_device->destroyShaderModule(compShader);
}

Pipeline::Pipeline(Pipeline&& other) noexcept :
    m_descriptorSetLayout(other.m_descriptorSetLayout),
    m_pipelineLayout(other.m_pipelineLayout),
    m_pipeline(other.m_pipeline) {
    other.m_descriptorSetLayout = nullptr;
    other.m_pipelineLayout = nullptr;
    other.m_pipeline = nullptr;
}

Pipeline& Pipeline::operator=(Pipeline&& other) noexcept {
    std::swap(m_descriptorSetLayout, other.m_descriptorSetLayout);
    std::swap(m_pipelineLayout, other.m_pipelineLayout);
    std::swap(m_pipeline, other.m_pipeline);
    return *this;
}

Pipeline::~Pipeline() {
    s_device->destroyPipeline(m_pipeline);
    s_device->destroyPipelineLayout(m_pipelineLayout);
    s_device->destroyDescriptorSetLayout(m_descriptorSetLayout);
}

void Pipeline::bind(vk::PipelineBindPoint bindPoint) const {
    s_commandBuffer->bindPipeline(bindPoint, m_pipeline);
}

void Pipeline::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) const {
    s_commandBuffer->draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

void Pipeline::drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) const {
    s_commandBuffer->drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void Pipeline::reflect(const ShaderSourceRef& src, vk::ShaderStageFlagBits st, std::vector<vk::DescriptorSetLayoutBinding>& dslbs, std::vector<vk::PushConstantRange>& ranges) const {
    auto compiler = spirv_cross::Compiler{src.vk13.data(), src.vk13.size()};
    auto reflectResources = [&](const spirv_cross::SmallVector<spirv_cross::Resource>& resources, vk::DescriptorType descType) {
        for (const auto& res : resources) {
            const auto& type = compiler.get_type(res.type_id);
            dslbs.emplace_back(
                compiler.get_decoration(res.id, spv::DecorationBinding),    //Binding
                descType,                                                   //Type
                type.array.empty() ? 1 : type.array[0],                     //Count
                st                                                          //Stages
            );
        }
    };
    auto resources = compiler.get_shader_resources();
    reflectResources(resources.uniform_buffers, vk::DescriptorType::eUniformBuffer);
    reflectResources(resources.storage_buffers, vk::DescriptorType::eStorageBuffer);
    reflectResources(resources.sampled_images, vk::DescriptorType::eCombinedImageSampler);
    reflectResources(resources.storage_images, vk::DescriptorType::eStorageImage);
    for (const auto& res : resources.push_constant_buffers) {               //Push constants
        const auto& type = compiler.get_type(res.base_type_id);
        ranges.emplace_back(
            st,                                                             //Stages
            0u,                                                             //Offset
            static_cast<uint32_t>(compiler.get_declared_struct_size(type))  //Size
        );
    }
}

}
