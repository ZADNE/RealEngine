/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>
#include <compare>
#include <string_view>
#include <initializer_list>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <vulkan/vulkan.hpp>

#include <RealEngine/utility/Error.hpp>
#include <RealEngine/rendering/buffers/BufferTypedIndex.hpp>
#include <RealEngine/rendering/textures/TextureUnit.hpp>
#include <RealEngine/rendering/textures/ImageUnit.hpp>

namespace RE {

/**
 * @brief Enum of all available shader stages.
*/
enum class ShaderType {
    VERTEX,
    TESS_CONTROL,
    TESS_EVALUATION,
    GEOMETRY,
    FRAGMENT,
    COMPUTE
};

/**
 * @brief Represents SPIR-V source code of a shader stage
*/
struct ShaderSource {
    const std::basic_string<uint32_t> vk13;/**< SPIR-V compiled for Vulkan 1.3 environment */
    const std::basic_string<uint32_t> gl46;/**< SPIR-V compiled for OpenGL 4.6 environment */
};

/**
 * @brief Represents a non-owning handle to source codes of a shader stage
*/
struct ShaderSourceRef {
    ShaderSourceRef() {}
    ShaderSourceRef(const ShaderSource& source) :
        vk13(source.vk13),
        gl46(source.gl46) {}

    std::basic_string_view<uint32_t> vk13{};
    std::basic_string_view<uint32_t> gl46{};
};

/**
* @brief POD representing source codes for all shaders within a shader program
*/
struct ShaderProgramSources {

    static constexpr size_t NUM_STAGES = 6;

    ShaderSourceRef operator[](size_t type) const {
        return operator[](static_cast<ShaderType>(type));
    }

    ShaderSourceRef operator[](ShaderType type) const {
        switch (type) {
        case ShaderType::VERTEX: return vert;
        case ShaderType::TESS_CONTROL: return tesc;
        case ShaderType::TESS_EVALUATION: return tese;
        case ShaderType::GEOMETRY: return geom;
        case ShaderType::FRAGMENT: return frag;
        case ShaderType::COMPUTE: return comp;
        default: throw Exception{"Tried to access invalid shader type"};
        }
    }

    ShaderSourceRef vert{};/**< Vertex shader stage of the program */
    ShaderSourceRef tesc{};/**< Tesselation control shader stage of the program */
    ShaderSourceRef tese{};/**< Tesselation evaluation stage of the program */
    ShaderSourceRef geom{};/**< Geometry shader stage of the program */
    ShaderSourceRef frag{};/**< Fragment shader stage of the program */
    ShaderSourceRef comp{};/**< Copute shader stage of the program */
};

/**
 * @brief Contains all members of ShaderProgram
 * @note For internal usage in RealEngine.
*/
class ShaderProgramID {
    friend class GL46ShaderProgram;
    friend class VK13ShaderProgram;
public:

    ~ShaderProgramID() = default;

    ShaderProgramID(const ShaderProgramID&) = delete;
    ShaderProgramID(ShaderProgramID&& other) noexcept :
        m_data(other.m_data) {
        std::memset(&other.m_data, 0, sizeof(other.m_data));
    }

    ShaderProgramID& operator=(const ShaderProgramID&) = delete;
    ShaderProgramID& operator=(ShaderProgramID&& other) noexcept {
        std::swap(m_data, other.m_data);
        return *this;
    }

private:

    ShaderProgramID(vk::Pipeline pipeline) :
        m_data({.pipeline = pipeline}) {}

    ShaderProgramID(unsigned int id) :
        m_data({.id = id}) {}

    union Data {
        vk::Pipeline pipeline;
        unsigned int id;
    } m_data;
};

/**
* @brief Is a renderer-agnostic interface to ShaderProgram's implementation.
*
* This is used internally by ShaderProgram.
*
* @see RE::ShaderProgram
*/
class IShaderProgram {
public:

    virtual ShaderProgramID construct(const ShaderProgramSources& sources) const = 0;
    virtual void destruct(ShaderProgramID& sp) const = 0;


    virtual void use(const ShaderProgramID& sp) const = 0;
    virtual void unuse(const ShaderProgramID& sp) const = 0;

    virtual void dispatchCompute(const ShaderProgramID& sp, const glm::uvec3& groupCount, bool use) const = 0;
    virtual void dispatchCompute(const ShaderProgramID& sp, int indirect, bool use) const = 0;

    virtual void printInfo(const ShaderProgramID& sp) const = 0;
    virtual void backInterfaceBlock(const ShaderProgramID& sp, unsigned int interfaceBlockIndex, const BufferTypedIndex& index) const = 0;

    virtual void setUniform(const ShaderProgramID& sp, int location, TextureUnit unit) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, ImageUnit unit) const = 0;
};

}
