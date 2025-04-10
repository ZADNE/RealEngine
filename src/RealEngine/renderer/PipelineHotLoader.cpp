/**
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/pipelines/Pipeline.hpp>
#include <RealEngine/renderer/PipelineHotLoader.hpp>

namespace re {

void PipelineHotLoader::registerForReloading(
    vk::Pipeline initial, const PipelineGraphicsCreateInfo& createInfo,
    const PipelineGraphicsSources& srcs
) {
    m_pipeToReloadInfo.try_emplace(initial, createInfo, srcs);
}

void PipelineHotLoader::registerForReloading(
    vk::Pipeline initial, const PipelineComputeCreateInfo& createInfo,
    const PipelineComputeSources& srcs
) {
    m_pipeToReloadInfo.try_emplace(initial, createInfo, srcs);
}

vk::Pipeline PipelineHotLoader::hotReload(
    vk::Pipeline current, vk::ShaderStageFlagBits stages
) {
    if (auto it = m_pipeToReloadInfo.find(current);
        it != m_pipeToReloadInfo.end()) {
        // Recompile GLSL sources to SPIR-V
        auto nodeHandle                = m_pipeToReloadInfo.extract(it);
        PipelineReloadInfo& reloadInfo = nodeHandle.mapped();

        // Recompile SPIR-V
        vk::Pipeline newPipeline = reloadInfo.recreateFromSources();

        // Delete old pipeline and return the new one
        m_deletionQueue.enqueueDeletion(nodeHandle.key());
        nodeHandle.key() = newPipeline;
        m_pipeToReloadInfo.insert(std::move(nodeHandle));
        return newPipeline;
    }
    return current;
}

void PipelineHotLoader::unregisterForReloading(vk::Pipeline current) {
    m_pipeToReloadInfo.erase(current);
}

vk::Pipeline PipelineHotLoader::PipelineReloadInfo::recreateFromSources() const {
    switch (type) {
    case PipelineType::Graphics:
        return Pipeline::create(
            graphics,
            PipelineGraphicsSources{
                .vert = sources[0],
                .tesc = sources[1],
                .tese = sources[2],
                .geom = sources[3],
                .frag = sources[4]
            }
        );
    case PipelineType::Compute:
        return Pipeline::create(compute, PipelineComputeSources{.comp = sources[0]});
    default: return nullptr;
    }
}

} // namespace re
