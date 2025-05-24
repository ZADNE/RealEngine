/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <functional>
#include <memory>

#include <RealEngine/graphics/pipelines/PipelineCreateInfos.hpp>
#include <RealEngine/graphics/pipelines/PipelineSources.hpp>
#include <RealEngine/renderer/DeletionQueue.hpp>
#include <RealEngine/resources/hot_reload/HotReloadInitInfo.hpp>

namespace re {

/**
 * @brief   Allows recompilation of Vulkan pipelines during runtime (debug
 *          builds only)
 */
class PipelineHotLoader {
public:

    PipelineHotLoader(DeletionQueue& deletionQueue, const HotReloadInitInfo& hotReload);

    PipelineHotLoader(const PipelineHotLoader&)            = default;
    PipelineHotLoader& operator=(const PipelineHotLoader&) = default;

    ~PipelineHotLoader(); ///< Defined in source file to enable PImpl idiom

    /**
     * @brief   Registers the pipeline so that it can be recompiled later
     * @note    Use moveRegisteredPipeline(...) if the pipeline should be
     *          relocated in memory.
     */
    void registerPipelineForReloading(
        vk::Pipeline& initial, const PipelineGraphicsCreateInfo& createInfo,
        const PipelineGraphicsSources& srcs
    );
    void registerPipelineForReloading(
        vk::Pipeline& initial, const PipelineComputeCreateInfo& createInfo,
        const PipelineComputeSources& srcs
    );

    /**
     * @brief Updates memory location of already registered pipeline
     */
    void moveRegisteredPipeline(vk::Pipeline& original, vk::Pipeline& moved);

    /**
     * @brief   Updates identifier of the pipeline
     * @details The identifier is returned in the callback when the pipeline is
     *          reloaded.
     */
    void setPipelineIdentifier(vk::Pipeline pipeline, int identifier);

    /**
     * @brief   Unregisters the pipeline
     * @details This does not destroy the pipeline. Does nothing if the
     *          pipeline has not been registered.
     */
    void unregisterPipelineForReloading(vk::Pipeline& pipeline);

    /**
     * @brief   Recreates all registered pipelines that had their sources changed
     * @details Must be called every frame.
     */
    void reloadChangedPipelines(const std::function<void(vk::Pipeline, int)>& reloadedCallback
    );

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
            vk::Pipeline& pipeline, const PipelineGraphicsCreateInfo& createInfo,
            const PipelineGraphicsSources& srcs
        )
            : m_pipeline{&pipeline}
            , m_type{PipelineType::Graphics}
            , m_graphicsCreateInfo{createInfo}
            , m_sources{srcs[0], srcs[1], srcs[2], srcs[3], srcs[4]} {}

        PipelineReloadInfo(
            vk::Pipeline& pipeline, const PipelineComputeCreateInfo& createInfo,
            const PipelineComputeSources& srcs
        )
            : m_pipeline{&pipeline}
            , m_type{PipelineType::Compute}
            , m_computeCreateInfo{createInfo}
            , m_sources{srcs[0], {}, {}, {}, {}} {}

        void updateTargetPipeline(vk::Pipeline& pipeline) {
            m_pipeline = &pipeline;
        }

        void setIdentifier(int indetifier) { m_identifier = indetifier; }
        int indentifier() const { return m_identifier; }

        vk::Pipeline targetPipeline() const { return *m_pipeline; }

        bool targetsPipeline(vk::Pipeline pipeline) const {
            return *m_pipeline == pipeline;
        }

        bool recreatePipelineFromSources(DeletionQueue& deletionQueue) const;

        std::span<ShaderSource> sources();

    private:
        vk::Pipeline* m_pipeline{};
        PipelineType m_type{};
        int m_identifier{};
        union {
            // It is dangerous to store the create infos as the pointers in them
            // may become dangling! It is accepted as this whole class is not
            // used in Release builds.
            PipelineGraphicsCreateInfo m_graphicsCreateInfo;
            PipelineComputeCreateInfo m_computeCreateInfo;
        };
        PipelineSources m_sources{};
    };

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
} // namespace re
