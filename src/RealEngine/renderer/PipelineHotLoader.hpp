/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <map>

#include <RealEngine/graphics/pipelines/PipelineCreateInfos.hpp>
#include <RealEngine/graphics/pipelines/PipelineSources.hpp>
#include <RealEngine/renderer/DeletionQueue.hpp>

namespace re {

/**
 * @brief Allows recompilation of Vulkan pipelines during runtime (debug builds only)
 */
class PipelineHotLoader {
public:

    explicit PipelineHotLoader(DeletionQueue& deletionQueue)
        : m_deletionQueue{deletionQueue} {}

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

    struct PipelineReloadInfo {
        PipelineReloadInfo(
            const PipelineGraphicsCreateInfo& createInfo,
            const PipelineGraphicsSources& srcs
        )
            : type{PipelineType::Graphics}
            , graphics{createInfo}
            , sources{srcs[0], srcs[1], srcs[2], srcs[3], srcs[4]} {}
        PipelineReloadInfo(
            const PipelineComputeCreateInfo& createInfo,
            const PipelineComputeSources& srcs
        )
            : type{PipelineType::Compute}
            , compute{createInfo}
            , sources{srcs[0], {}, {}, {}, {}} {}

        vk::Pipeline recreateFromSources() const;

        PipelineType type{};
        union {
            // It is dangerous to store the create infos as the pointers in them
            // may become dangling! It is accepted as this whole class is not
            // used in Release builds.
            PipelineGraphicsCreateInfo graphics;
            PipelineComputeCreateInfo compute;
        };
        PipelineSources sources{};
    };

    DeletionQueue& m_deletionQueue;
    std::map<vk::Pipeline, PipelineReloadInfo> m_pipeToReloadInfo;
};

} // namespace re
