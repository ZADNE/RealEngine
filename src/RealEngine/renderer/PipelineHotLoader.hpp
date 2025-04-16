/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <map>
#include <memory>

#include <RealEngine/graphics/pipelines/PipelineCreateInfos.hpp>
#include <RealEngine/graphics/pipelines/PipelineSources.hpp>
#include <RealEngine/renderer/DeletionQueue.hpp>

namespace re {

/**
 * @brief Allows recompilation of Vulkan pipelines during runtime (debug
 * builds only)
 */
class PipelineHotLoader {
public:

    explicit PipelineHotLoader(DeletionQueue& deletionQueue);

    /**
     * @brief   Registers the pipeline so that it can be recompiled later
     */
    void registerForReloading(
        vk::Pipeline initial, const PipelineGraphicsCreateInfo& createInfo,
        const PipelineGraphicsSources& srcs
    );
    void registerForReloading(
        vk::Pipeline initial, const PipelineComputeCreateInfo& createInfo,
        const PipelineComputeSources& srcs
    );

    /**
     * @brief   Recompiles stages of the pipeline from GLSL source in filesystem
     * @details The pipeline must have been register previously with
     *          registerForReloading() or it must have been already reloaded
     *          (that is returned by this function).
     *          If realoded, the previous pipeline has been scheduled for deletion.
     * @return  The recompiled pipeline or the current one if it has not been
     *          registered before.
     */
    vk::Pipeline hotReload(vk::Pipeline current, vk::ShaderStageFlagBits stages);

    /**
     * @brief   Unregisters the pipeline
     * @details This does not destroy the pipeline. Does nothing if the pipeline
     *          has not been registered.
     */
    void unregisterForReloading(vk::Pipeline current);

private:
    enum class PipelineType {
        Graphics,
        Compute
    };
    static constexpr size_t k_maxStagesPerPipeline = std::max(
        PipelineGraphicsSources::k_numStages, PipelineComputeSources::k_numStages
    );
    using PipelineSources = std::array<ShaderSource, k_maxStagesPerPipeline>;

    class PipelineReloadInfo {
    public:
        PipelineReloadInfo(
            const PipelineGraphicsCreateInfo& createInfo,
            const PipelineGraphicsSources& srcs
        )
            : m_type{PipelineType::Graphics}
            , m_graphics{createInfo}
            , m_sources{srcs[0], srcs[1], srcs[2], srcs[3], srcs[4]} {}
        PipelineReloadInfo(
            const PipelineComputeCreateInfo& createInfo,
            const PipelineComputeSources& srcs
        )
            : m_type{PipelineType::Compute}
            , m_compute{createInfo}
            , m_sources{srcs[0], {}, {}, {}, {}} {}

        void reloadSPIRV(vk::ShaderStageFlagBits stages) {
            switch (m_type) {
            case PipelineType::Graphics:
                reloadSPIRV<PipelineGraphicsSources>(stages);
                break;
            case PipelineType::Compute:
                reloadSPIRV<PipelineComputeSources>(stages);
                break;
            default: std::unreachable();
            }
        }

        template<typename T>
        void reloadSPIRV(vk::ShaderStageFlagBits stages);

        vk::Pipeline recreatePipelineFromSPIRV() const;

    private:
        PipelineType m_type{};
        union {
            // It is dangerous to store the create infos as the pointers in them
            // may become dangling! It is accepted as this whole class is not
            // used in Release builds.
            PipelineGraphicsCreateInfo m_graphics;
            PipelineComputeCreateInfo m_compute;
        };
        PipelineSources m_sources{};
    };

    DeletionQueue& m_deletionQueue;
    std::map<vk::Pipeline, PipelineReloadInfo> m_pipeToReloadInfo;
};
} // namespace re
