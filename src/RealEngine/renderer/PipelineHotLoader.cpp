/**
 *  @author    Dubsky Tomas
 */
#include <RealEngine/graphics/pipelines/Pipeline.hpp>
#include <RealEngine/renderer/PipelineHotLoader.hpp>
#include <RealEngine/resources/FileIO.hpp>
#include <RealEngine/utility/details/CMakeConstants.hpp>

namespace re {

PipelineHotLoader::PipelineHotLoader(
    DeletionQueue& deletionQueue, const HotReloadInitInfo& hotReload
)
    : m_deletionQueue{deletionQueue}
    , m_recompileShadersCommand{std::format(
          "{} --build {} -t {}{}", hotReload.cmakePath, hotReload.targetBinaryDir,
          hotReload.targetName, details::k_shaderTargetSuffix
      )}
    , m_binaryDir{hotReload.targetBinaryDir} {
}

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
        // Reload SPIR-V from filesystem
        auto nodeHandle                = m_pipeToReloadInfo.extract(it);
        PipelineReloadInfo& reloadInfo = nodeHandle.mapped();
        reloadInfo.reloadSPIRV(m_binaryDir, stages);

        // Recompile SPIR-V
        vk::Pipeline newPipeline = reloadInfo.recreatePipelineFromSPIRV();

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

template<typename T>
void PipelineHotLoader::PipelineReloadInfo::reloadSPIRV(
    const std::string& binaryDir, vk::ShaderStageFlagBits stages
) {
    std::vector<unsigned char> temp;
    for (size_t i = 0; i < T::k_numStages; ++i) {
        vk::ShaderStageFlagBits thisStage = T::stageFlags(i);
        if ((thisStage & stages) && m_sources[i].relPath) {
            readBinaryFile(
                std::format(
                    "{}/{}.{}", binaryDir,
                    static_cast<const char*>(m_sources[i].relPath),
                    details::k_shaderSPIRVBinFileExt
                ),
                temp
            );
            assert((temp.size() % sizeof(uint32_t)) == 0);
            m_sources[i].vk13.assign(
                reinterpret_cast<const uint32_t*>(temp.data()), temp.size() / 4
            );
            int stop = 5;
        }
    }
}

vk::Pipeline PipelineHotLoader::PipelineReloadInfo::recreatePipelineFromSPIRV() const {
    switch (m_type) {
    case PipelineType::Graphics:
        return Pipeline::create(
            m_graphics,
            PipelineGraphicsSources{
                .vert = m_sources[0],
                .tesc = m_sources[1],
                .tese = m_sources[2],
                .geom = m_sources[3],
                .frag = m_sources[4]
            }
        );
    case PipelineType::Compute:
        return Pipeline::create(
            m_compute, PipelineComputeSources{.comp = m_sources[0]}
        );
    default: std::unreachable();
    }
}

} // namespace re
