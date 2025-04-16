/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>
#include <string_view>

#include <vulkan/vulkan.hpp>

#include <RealEngine/utility/DebugString.hpp>
#include <RealEngine/utility/Error.hpp>

namespace re {

struct ShaderSource;

/**
 * @brief Represents a non-owning handle to source codes of a shader stage
 */
struct ShaderSourceRef {
    constexpr ShaderSourceRef() {}
    constexpr ShaderSourceRef(const ShaderSource& source);

    std::basic_string_view<uint32_t> vk13{};
    [[no_unique_address]] DebugString<> sourcePath{};
};

/**
 * @brief Represents SPIR-V source code of a shader stage
 */
struct ShaderSource {
    constexpr ShaderSource() {}
    constexpr ShaderSource(
        std::initializer_list<uint32_t> vk13_, [[maybe_unused]] const char* sourcePath_
    )
        : vk13{vk13_}
        , sourcePath{sourcePath_} {}
    constexpr ShaderSource(const ShaderSourceRef& sourceRef)
        : vk13{sourceRef.vk13}
        , sourcePath{sourceRef.sourcePath} {}

    /**
     * @brief SPIR-V compiled for Vulkan 1.3 environment
     */
    std::basic_string<uint32_t> vk13{};

    /**
     * @brief Full path to the GLSL source file of the shader (debug only)
     */
    [[no_unique_address]] DebugString<> sourcePath{};
};

constexpr ShaderSourceRef::ShaderSourceRef(const ShaderSource& source)
    : vk13{source.vk13}
    , sourcePath{source.sourcePath} {
}

/**
 * @brief POD representing source codes for all shaders within a graphics pipeline
 */
struct PipelineGraphicsSources {

    static constexpr size_t k_numStages = 5;

    ShaderSourceRef operator[](size_t type) const {
        switch (type) {
        case 0:  return vert;
        case 1:  return tesc;
        case 2:  return tese;
        case 3:  return geom;
        case 4:  return frag;
        default: throw Exception{"Tried to access invalid shader type"};
        }
    }

    static vk::ShaderStageFlagBits stageFlags(size_t type) {
        using enum vk::ShaderStageFlagBits;
        switch (type) {
        case 0:  return eVertex;
        case 1:  return eTessellationControl;
        case 2:  return eTessellationEvaluation;
        case 3:  return eGeometry;
        case 4:  return eFragment;
        default: throw Exception{"Tried to access invalid shader type"};
        }
    }

    ShaderSourceRef operator[](vk::ShaderStageFlagBits type) const {
        using enum vk::ShaderStageFlagBits;
        switch (type) {
        case eVertex:                 return vert;
        case eTessellationControl:    return tesc;
        case eTessellationEvaluation: return tese;
        case eGeometry:               return geom;
        case eFragment:               return frag;
        default:                      throw Exception{"Tried to access invalid shader type"};
        }
    }

    ShaderSourceRef vert{}; ///< Vertex shader stage of the program
    ShaderSourceRef tesc{}; ///< Tesselation control shader stage of the program
    ShaderSourceRef tese{}; ///< Tesselation evaluation stage of the program
    ShaderSourceRef geom{}; ///< Geometry shader stage of the program
    ShaderSourceRef frag{}; ///< Fragment shader stage of the program
};

/**
 * @brief POD representing source codes for all shaders within a compute pipeline
 */
struct PipelineComputeSources {

    static constexpr size_t k_numStages = 1;

    ShaderSourceRef operator[](size_t type) const {
        switch (type) {
        case 0:  return comp;
        default: throw Exception{"Tried to access invalid shader type"};
        }
    }

    static vk::ShaderStageFlagBits stageFlags(size_t type) {
        using enum vk::ShaderStageFlagBits;
        switch (type) {
        case 0:  return eCompute;
        default: throw Exception{"Tried to access invalid shader type"};
        }
    }

    ShaderSourceRef operator[](vk::ShaderStageFlagBits type) const {
        using enum vk::ShaderStageFlagBits;
        switch (type) {
        case eCompute: return comp;
        default:       throw Exception{"Tried to access invalid shader type"};
        }
    }

    ShaderSourceRef comp{}; ///< Compute shader stage of the program
};

} // namespace re
