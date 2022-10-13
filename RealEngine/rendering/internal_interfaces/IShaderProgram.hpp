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
using ShaderSource = const std::basic_string<int>;

/**
 * @brief Represents a non-owning handle to source codes of a shader stage
*/
using ShaderSourceRef = std::basic_string_view<int>;

/**
* @brief POD representing source codes for all shaders within a shader program
*/
struct ShaderProgramSources {

    ShaderSourceRef operator[](ShaderType type) const {
        switch (type) {
        case RE::ShaderType::VERTEX: return vert;
        case RE::ShaderType::TESS_CONTROL: return tesc;
        case RE::ShaderType::TESS_EVALUATION: return tese;
        case RE::ShaderType::GEOMETRY: return geom;
        case RE::ShaderType::FRAGMENT: return frag;
        case RE::ShaderType::COMPUTE: return comp;
        default: fatalError("Tried to access invalid shader type");
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
        m_id(other.m_id) {
        other.m_id = 0;
    }

    ShaderProgramID& operator=(const ShaderProgramID&) = delete;
    ShaderProgramID& operator=(ShaderProgramID&& other) noexcept {
        std::swap(m_id, other.m_id);
        return *this;
    }

private:

    ShaderProgramID(unsigned int id) :
        m_id(id) {}

    unsigned int m_id = 0;      /**< Internal identifier */
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

    virtual int getUniformLocation(const ShaderProgramID& sp, const std::string& name) const = 0;

    virtual void setUniform(const ShaderProgramID& sp, int location, float val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, float val0, float val1) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, const glm::vec2& val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, float val0, float val1, float val2) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, const glm::vec3& val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, float val0, float val1, float val2, float val3) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, const glm::vec4& val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, int count, const float* val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, int count, const glm::vec2* val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, int count, const glm::vec3* val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, int count, const glm::vec4* val) const = 0;

    virtual void setUniform(const ShaderProgramID& sp, int location, int val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, int val0, int val1) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, const glm::ivec2& val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, int val0, int val1, int val2) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, const glm::ivec3& val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, int val0, int val1, int val2, int val3) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, const glm::ivec4& val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, int count, const int* val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, int count, const glm::ivec2* val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, int count, const glm::ivec3* val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, int count, const glm::ivec4* val) const = 0;

    virtual void setUniform(const ShaderProgramID& sp, int location, unsigned int val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, unsigned int val0, unsigned int val1) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, const glm::uvec2& val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, unsigned int val0, unsigned int val1, unsigned int val2) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, const glm::uvec3& val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, unsigned int val0, unsigned int val1, unsigned int val2, unsigned int val3) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, const glm::uvec4& val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, int count, const unsigned int* val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, int count, const glm::uvec2* val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, int count, const glm::uvec3* val) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, int count, const glm::uvec4* val) const = 0;

    virtual void setUniform(const ShaderProgramID& sp, int location, int count, const glm::mat4* val) const = 0;

    virtual void setUniform(const ShaderProgramID& sp, int location, TextureUnit unit) const = 0;
    virtual void setUniform(const ShaderProgramID& sp, int location, ImageUnit unit) const = 0;
};

}
