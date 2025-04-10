﻿/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/graphics/pipelines/PipelineCreateInfos.hpp>
#include <RealEngine/graphics/pipelines/PipelineSources.hpp>
#include <RealEngine/renderer/VulkanObjectBase.hpp>

namespace re {

/**
 * @brief Controls how vertices are transformed and shaded or describes compute work
 */
class Pipeline: public VulkanObjectBase {
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

    /**
     * @brief Recompiles shaders of the pipeline from file system
     * @note  Only supported in Debug builds, does nothing in other builds
     */
    void hotReloadShaders(vk::ShaderStageFlagBits stages);

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
