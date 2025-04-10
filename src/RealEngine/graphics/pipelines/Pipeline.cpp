/**
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/pipelines/Pipeline.hpp>

using enum vk::BlendFactor;
using enum vk::BlendOp;
using enum vk::ColorComponentFlagBits;

namespace re {

namespace {
vk::ShaderStageFlagBits convert(size_t st) {
    switch (st) {
    case 0:  return vk::ShaderStageFlagBits::eVertex;
    case 1:  return vk::ShaderStageFlagBits::eTessellationControl;
    case 2:  return vk::ShaderStageFlagBits::eTessellationEvaluation;
    case 3:  return vk::ShaderStageFlagBits::eGeometry;
    case 4:  return vk::ShaderStageFlagBits::eFragment;
    default: throw Exception{"Unknown shader type"};
    }
}
constexpr vk::PipelineVertexInputStateCreateInfo k_emptyVertexInput{};
} // namespace

Pipeline::Pipeline(
    const PipelineGraphicsCreateInfo& createInfo, const PipelineGraphicsSources& srcs
)
    : m_pipeline{create(createInfo, srcs)} {
    if constexpr (k_buildType == BuildType::Debug) {
        pipelineHotLoader().registerForReloading(m_pipeline, createInfo, srcs);
    }
}

Pipeline::Pipeline(
    const PipelineComputeCreateInfo& createInfo, const PipelineComputeSources& srcs
)
    : m_pipeline{create(createInfo, srcs)} {
    if constexpr (k_buildType == BuildType::Debug) {
        pipelineHotLoader().registerForReloading(m_pipeline, createInfo, srcs);
    }
}

Pipeline::Pipeline(Pipeline&& other) noexcept
    : m_pipeline{std::exchange(other.m_pipeline, nullptr)} {
}

Pipeline& Pipeline::operator=(Pipeline&& other) noexcept {
    std::swap(m_pipeline, other.m_pipeline);
    return *this;
}

Pipeline::~Pipeline() {
    if constexpr (k_buildType == BuildType::Debug) {
        pipelineHotLoader().unregisterForReloading(m_pipeline);
    }
    deletionQueue().enqueueDeletion(m_pipeline);
}

void Pipeline::hotReloadShaders(vk::ShaderStageFlagBits stages) {
    if constexpr (k_buildType == BuildType::Debug) {
        m_pipeline = pipelineHotLoader().hotReload(m_pipeline, stages);
    }
}

vk::Pipeline Pipeline::create(
    const PipelineGraphicsCreateInfo& createInfo, const PipelineGraphicsSources& srcs
) {
    // Create shader modules
    constexpr auto k_numStages = PipelineGraphicsSources::k_numStages;
    std::array<vk::ShaderModule, k_numStages> modules;
    std::array<vk::PipelineShaderStageCreateInfo, k_numStages> stages;
    uint32_t shaderCount = 0;
    for (size_t st = 0; st < k_numStages; ++st) {
        if (!srcs[st].vk13.empty()) {
            modules[shaderCount] = device().createShaderModule(
                {{}, srcs[st].vk13.size() * 4, srcs[st].vk13.data()}
            );
            stages[shaderCount] = vk::PipelineShaderStageCreateInfo{
                {}, convert(st), modules[shaderCount], "main", createInfo.specializationInfo
            };
            shaderCount++;
        }
    }
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{
        {}, createInfo.topology, createInfo.enablePrimitiveRestart
    };
    vk::PipelineTessellationStateCreateInfo tessellation{
        {}, createInfo.patchControlPoints
    };
    vk::PipelineViewportStateCreateInfo viewport{
        {},
        1u,
        nullptr, // Viewport
        1u,
        nullptr  // Scissor
    };
    vk::PipelineRasterizationStateCreateInfo rasterization{
        {},
        false, // Enable depth clamping
        false, // Enable rasterizer discard
        createInfo.polygonMode,
        createInfo.cullMode,
        vk::FrontFace::eClockwise,
        false,               // Enable depth bias
        0.0f,                // Depth bias constant factor
        0.0f,                // Depth bias clamp
        0.0f,                // Depth bias slope factor
        createInfo.lineWidth // Line width
    };
    vk::PipelineMultisampleStateCreateInfo multisample{};
    vk::PipelineDepthStencilStateCreateInfo depthStencil{
        {}, createInfo.enableDepth, createInfo.enableDepth, vk::CompareOp::eLess
    };
    vk::PipelineColorBlendAttachmentState colorBlendAttachment{
        createInfo.enableBlend,
        eSrcAlpha,         // Src color
        eOneMinusSrcAlpha, // Dst color
        eAdd,              // Color operation
        eOne,              // Src alpha
        eOneMinusSrcAlpha, // Dst alpha
        eAdd,              // Alpha operation
        eR | eG | eB | eA  // Write mask
    };
    vk::PipelineColorBlendStateCreateInfo colorBlend{
        {},
        false,
        vk::LogicOp::eClear, // Logic op (disable)
        colorBlendAttachment
    };
    std::array dynamicStates = std::to_array<vk::DynamicState>(
        {vk::DynamicState::eViewport, vk::DynamicState::eScissor}
    );
    vk::PipelineDynamicStateCreateInfo dynamic{{}, dynamicStates};

    // Create pipeline
    vk::Pipeline pipeline =
        device()
            .createGraphicsPipeline(
                pipelineCache(),
                vk::GraphicsPipelineCreateInfo{
                    {},
                    shaderCount,
                    stages.data(), // Shader modules
                    createInfo.vertexInput ? createInfo.vertexInput : &k_emptyVertexInput,
                    &inputAssembly,
                    &tessellation,
                    &viewport,
                    &rasterization,
                    &multisample,
                    &depthStencil,
                    &colorBlend,
                    &dynamic,
                    createInfo.pipelineLayout,
                    createInfo.renderPassSubpass.renderPass,
                    createInfo.renderPassSubpass.subpassIndex,
                    nullptr,
                    -1 // No base pipeline
                }
            )
            .value;

    // Destroy shader modules
    for (uint32_t i = 0; i < shaderCount; i++) {
        device().destroyShaderModule(modules[i]);
    }

    setDebugUtilsObjectName(pipeline, createInfo.debugName);

    return pipeline;
}

vk::Pipeline Pipeline::create(
    const PipelineComputeCreateInfo& createInfo, const PipelineComputeSources& srcs
) {
    // Create compute shader module
    vk::ShaderModule compShader = device().createShaderModule(vk::ShaderModuleCreateInfo{
        {}, srcs.comp.vk13.size() * 4, srcs.comp.vk13.data()
    });

    // Create pipeline
    vk::Pipeline pipeline =
        device()
            .createComputePipeline(
                pipelineCache(),
                vk::ComputePipelineCreateInfo{
                    {},
                    vk::PipelineShaderStageCreateInfo{
                        {},
                        vk::ShaderStageFlagBits::eCompute,
                        compShader,
                        "main",
                        createInfo.specializationInfo
                    },
                    createInfo.pipelineLayout,
                    nullptr,
                    -1 // No base pipeline
                }
            )
            .value;

    // Destroy compute shader module
    device().destroyShaderModule(compShader);

    setDebugUtilsObjectName(pipeline, createInfo.debugName);

    return pipeline;
}

} // namespace re
