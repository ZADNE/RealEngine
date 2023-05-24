/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/pipelines/PipelineLayout.hpp>

#include <spirv_glsl.hpp>

namespace RE {

PipelineLayout::PipelineLayout(const PipelineLayoutCreateInfo& createInfo, const PipelineGraphicsSources& srcs)
    : PipelineLayout(createInfo, reflectSources(srcs, createInfo.specializationInfo)) {
}

PipelineLayout::PipelineLayout(const PipelineLayoutCreateInfo& createInfo, const PipelineComputeSources& srcs)
    : PipelineLayout(createInfo, reflectSources(srcs, createInfo.specializationInfo)) {
}

PipelineLayout::PipelineLayout(const PipelineLayoutCreateInfo& createInfo, const PipelineLayoutDescription& description) {
    //Create descriptor sets
    m_descriptorSetLayouts.reserve(description.bindings.size());
    for (size_t i = 0; i < description.bindings.size(); i++) {
        //Gets flags for this descriptor set
        uint32_t flagsCount = 0u;
        const vk::DescriptorBindingFlags* flags = nullptr;
        if (i < createInfo.descriptorBindingFlags.size()) {
            flagsCount = static_cast<uint32_t>(createInfo.descriptorBindingFlags.data()[i].size());
            if (flagsCount > 0) {
                flags = createInfo.descriptorBindingFlags.data()[i].data();
            }
        }
        //Create this descriptor set
        m_descriptorSetLayouts.emplace_back(device().createDescriptorSetLayout(
            vk::StructureChain{
                vk::DescriptorSetLayoutCreateInfo{{},
                    description.bindings[i]
                },
                vk::DescriptorSetLayoutBindingFlagsCreateInfo{
                    flagsCount,
                    flags
                }
            }.get<>())
        );
    }
    //Create pipeline layout
    m_pipelineLayout = device().createPipelineLayout(vk::PipelineLayoutCreateInfo{{}, m_descriptorSetLayouts, description.ranges});
}

PipelineLayout::PipelineLayout(PipelineLayout&& other) noexcept:
    m_descriptorSetLayouts(other.m_descriptorSetLayouts),
    m_pipelineLayout(other.m_pipelineLayout) {
    other.m_descriptorSetLayouts.clear();
    other.m_pipelineLayout = nullptr;
}

PipelineLayout& PipelineLayout::operator=(PipelineLayout&& other) noexcept {
    std::swap(m_descriptorSetLayouts, other.m_descriptorSetLayouts);
    std::swap(m_pipelineLayout, other.m_pipelineLayout);
    return *this;
}

PipelineLayout::~PipelineLayout() {
    device().destroyPipelineLayout(m_pipelineLayout);
    for (int i = static_cast<int>(m_descriptorSetLayouts.size()) - 1; i >= 0; i--) {
        device().destroyDescriptorSetLayout(m_descriptorSetLayouts[i]);
    }
}

void PipelineLayout::reflectSource(
    const ShaderSourceRef& src,
    vk::ShaderStageFlagBits st,
    const vk::SpecializationInfo& specInfo,
    PipelineLayoutDescription& description
) const {
    //Run compiler
    auto compiler = spirv_cross::Compiler{src.vk13.data(), src.vk13.size()};
    //Override specialization constants
    auto constants = compiler.get_specialization_constants();
    const char* specData = reinterpret_cast<const char*>(specInfo.pData);
    for (const auto& constant : constants) {                                //For each spec constant in the SPIRV
        for (uint32_t i = 0; i < specInfo.mapEntryCount; i++) {             //Search its override entry
            const auto& map = specInfo.pMapEntries[i];
            if (constant.constant_id == map.constantID) {                   //If it is the override entry
                auto& c = compiler.get_constant(constant.id);
                std::memcpy(&c.m.c[0].r, &specData[map.offset], map.size);  //Override the constant
                break;
            }
        }
    }
    //Build descriptor layout
    auto reflectResources = [&](const spirv_cross::SmallVector<spirv_cross::Resource>& resources, vk::DescriptorType descType) {
        for (const auto& res : resources) {
            const auto& type = compiler.get_type(res.type_id);
            uint32_t count = type.array.empty() ? 1 :                       //Not array
                (type.array_size_literal[0] ? type.array[0] :               //Array with literal size
                compiler.get_constant(type.array[0]).m.c[0].r->u32);        //Array with spec const size
            auto set = compiler.get_decoration(res.id, spv::DecorationDescriptorSet);
            while (set >= description.bindings.size()) {                     //Emplace empty descriptor set layout
                description.bindings.emplace_back();
            }
            description.bindings[set].emplace_back(                          //Emplace binding to correct set layout
                compiler.get_decoration(res.id, spv::DecorationBinding),    //Binding
                descType,                                                   //Type
                count,                                                      //Count
                st                                                          //Stages
            );
        }
    };
    auto resources = compiler.get_shader_resources();
    reflectResources(resources.uniform_buffers, vk::DescriptorType::eUniformBuffer);
    reflectResources(resources.storage_buffers, vk::DescriptorType::eStorageBuffer);
    reflectResources(resources.sampled_images, vk::DescriptorType::eCombinedImageSampler);
    reflectResources(resources.storage_images, vk::DescriptorType::eStorageImage);
    //Build push constant range
    for (const auto& res : resources.push_constant_buffers) {               //Push constants
        const auto& type = compiler.get_type(res.base_type_id);
        description.ranges.emplace_back(
            st,                                                             //Stages
            0u,                                                             //Offset
            static_cast<uint32_t>(compiler.get_declared_struct_size(type))  //Size
        );
    }
}

}
