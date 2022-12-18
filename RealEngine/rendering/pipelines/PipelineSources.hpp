/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>
#include <string_view>

#include <vulkan/vulkan.hpp>

#include <RealEngine/utility/Error.hpp>

namespace RE {

/**
 * @brief Represents SPIR-V source code of a shader stage
*/
struct ShaderSource {
    const std::basic_string<uint32_t> vk13;/**< SPIR-V compiled for Vulkan 1.3 environment */
};

/**
 * @brief Represents a non-owning handle to source codes of a shader stage
*/
struct ShaderSourceRef {
    ShaderSourceRef() {}
    ShaderSourceRef(const ShaderSource& source) :
        vk13(source.vk13) {}

    std::basic_string_view<uint32_t> vk13{};
};

/**
* @brief POD representing source codes for all shaders within a pipeline
*/
struct PipelineSources {

    static constexpr size_t NUM_STAGES = 5;

    ShaderSourceRef operator[](size_t type) const {
        switch (type) {
        case 0: return vert;
        case 1: return tesc;
        case 2: return tese;
        case 3: return geom;
        case 4: return frag;
            //case 5: return comp;
        default: throw Exception{"Tried to access invalid shader type"};
        }
    }

    ShaderSourceRef operator[](vk::ShaderStageFlagBits type) const {
        using enum vk::ShaderStageFlagBits;
        switch (type) {
        case eVertex: return vert;
        case eTessellationControl: return tesc;
        case eTessellationEvaluation: return tese;
        case eGeometry: return geom;
        case eFragment: return frag;
            //case eCompute: return comp;
        default: throw Exception{"Tried to access invalid shader type"};
        }
    }

    ShaderSourceRef vert{};/**< Vertex shader stage of the program */
    ShaderSourceRef tesc{};/**< Tesselation control shader stage of the program */
    ShaderSourceRef tese{};/**< Tesselation evaluation stage of the program */
    ShaderSourceRef geom{};/**< Geometry shader stage of the program */
    ShaderSourceRef frag{};/**< Fragment shader stage of the program */
    //ShaderSourceRef comp{};/**< Copute shader stage of the program */
};

}
