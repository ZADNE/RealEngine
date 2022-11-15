/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <RealEngine/rendering/internal_interfaces/IShaderProgram.hpp>
#include <RealEngine/rendering/Renderer.hpp>


namespace RE {

/**
 * @brief Controls how vertices are rendered to screen.
 * @tparam R The renderer that will perform the commands
*/
template<Renderer R = RendererLateBind>
class ShaderProgram {
    friend class GL46Fixture; friend class VK13Fixture;
public:

    /**
     * @brief Constructs shader program from given source codes
     * @param sources Source codes of the program
    */
    ShaderProgram(const ShaderProgramSources& sources);

    ShaderProgram(const ShaderProgram<R>&) = delete;
    ShaderProgram(ShaderProgram<R>&& other) noexcept;

    ShaderProgram<R>& operator=(const ShaderProgram<R>&) = delete;
    ShaderProgram<R>& operator=(ShaderProgram<R>&& other) noexcept;

    /**
     * @brief Destructs shader program
    */
    ~ShaderProgram();

    /**
    * @brief Sets the shader active for rendering
    */
    void use() const;

    /**
    * @brief Unsets the shader for rendering
    */
    void unuse() const;

    /**
     * @brief Dispatches compute groups of this shader program.
     *
     * Can be used only if the program was compiled with compute shader stage.
     *
     * @param groupCount The number of work groups to be launched in each.
     * @param use If true, the program is bound and unbound for usage. If false it is expected to be already bound.
    */
    void dispatchCompute(const glm::uvec3& groupCount, bool use) const;

    /**
     * @brief Dispatches indirectly compute groups of this shader program.
     * @param indirect The offset into the buffer object currently bound as the DISPATCH_INDIRECT buffer
     * @param use If true, the program is bound and unbound for usage. If false it is expected to be already bound.
    */
    void dispatchCompute(int indirect, bool use) const;

    /**
    * @brief Prints diagnostic information about the shader program
    */
    void printInfo() const;

    /**
     * @brief Backs the interface block with a buffer indexed binding point
     * @param interfaceBlockIndex Index of the interface block within this program
     * @param index The typed index of the buffer that will back the interface block. Only UNIFORM and SHADER_STORAGE are available.
    */
    void backInterfaceBlock(unsigned int interfaceBlockIndex, const BufferTypedIndex& index) const;

    void setUniform(int location, TextureUnit unit) const;                              /**< Sets sampler uniform at given location */
    void setUniform(int location, ImageUnit unit) const;                                /**< Sets image uniform at given location */

private:

    ShaderProgramID m_id;

    static inline R::ShaderProgram* s_impl = nullptr;
};

}