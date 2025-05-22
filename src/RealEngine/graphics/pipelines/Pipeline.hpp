/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/graphics/pipelines/PipelineCreateInfos.hpp>
#include <RealEngine/graphics/pipelines/PipelineSources.hpp>
#include <RealEngine/renderer/ObjectUsingVulkan.hpp>

namespace re {

/**
 * @brief Controls how vertices are transformed and shaded or describes compute work
 */
class Pipeline: public ObjectUsingVulkan {
    friend class PipelineHotLoader;
public:
    /**
     * @brief Constructs a null pipeline that cannot be used for rendering or compute
     */
    explicit Pipeline() {}

    /**
     * @brief Constructs graphics pipeline
     */
    Pipeline(
        const PipelineGraphicsCreateInfo& createInfo,
        const PipelineGraphicsSources& srcs
    );

    /**
     * @brief Constructs compute pipeline
     */
    Pipeline(
        const PipelineComputeCreateInfo& createInfo, const PipelineComputeSources& srcs
    );

    Pipeline(const Pipeline&)            = delete;  ///< Noncopyable
    Pipeline& operator=(const Pipeline&) = delete;  ///< Noncopyable

    Pipeline(Pipeline&& other) noexcept;            ///< Movable
    Pipeline& operator=(Pipeline&& other) noexcept; ///< Movable

    ~Pipeline();

    const vk::Pipeline& operator*() const { return m_pipeline; }
    const vk::Pipeline* operator->() const { return &m_pipeline; }

    const vk::Pipeline& pipeline() const { return m_pipeline; }

#if RE_BUILDING_FOR_DEBUG
    /**
     * @brief   Updates hot realod identifier
     * @details 0 is the default value if not changed
     */
    void setHotReloadIdentifier(int identifier);
#endif // RE_BUILDING_FOR_DEBUG

private:
    static vk::Pipeline create(
        const PipelineGraphicsCreateInfo& createInfo,
        const PipelineGraphicsSources& srcs
    );
    static vk::Pipeline create(
        const PipelineComputeCreateInfo& createInfo, const PipelineComputeSources& srcs
    );

    vk::Pipeline m_pipeline{};
};

} // namespace re
