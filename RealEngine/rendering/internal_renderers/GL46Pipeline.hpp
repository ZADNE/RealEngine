/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IPipeline.hpp>

namespace RE {

/**
* @brief Is OpenGL 4.6 implementation of Pipeline class.
*
* Do not use this directly - use Pipeline instead.
*/
class GL46Pipeline final : public IPipeline {
public:

    PipelineID construct(/*const std::span<const BufferID>& vbs, */const vk::PipelineVertexInputStateCreateInfo& vi, const ShaderProgramSources& srcs) const override;
    void destruct(PipelineID& pl) const override;

private:

    unsigned int createVertexArray(const std::span<const BufferID>& vbs, const vk::PipelineVertexInputStateCreateInfo& vi) const;
    unsigned int createShaderProgram(const ShaderProgramSources& sources) const;
};

}