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
    friend class GL46Fixture;
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

    /**
     * @brief Resolves location of uniform "name"
     * @param name Name of the uniform
     * @return Location of the uniform, negative if there is no such uniform
    */
    int getUniformLocation(const std::string& name) const;

    /**
     * @brief Resolves location of uniform "name" and sets its value
     * @param name Name of the uniform
     * 
     * This function has considerable overhead compared to overloads with integer location.
     * You should not use it in the main loop.
    */
    template<typename... Args>
    void setUniform(const std::string& name, Args... args) const {
        auto loc = getUniformLocation(name);
    #ifdef _DEBUG
        if (loc < 0) error(std::string{"Uniform \""} + name + "\" does not exist in the shader program!");
    #endif // _DEBUG
        setUniform(loc, args...);
    }

    void setUniform(int location, float val) const;                                     /**< Sets float uniform at given location */
    void setUniform(int location, float val0, float val1) const;                        /**< Sets vec2 uniform at given location */
    void setUniform(int location, const glm::vec2& val) const;                          /**< Sets vec2 uniform at given location */
    void setUniform(int location, float val0, float val1, float val2) const;            /**< Sets vec3 uniform at given location */
    void setUniform(int location, const glm::vec3& val) const;                          /**< Sets vec3 uniform at given location */
    void setUniform(int location, float val0, float val1, float val2, float val3) const;/**< Sets vec4 uniform at given location */
    void setUniform(int location, const glm::vec4& val) const;                          /**< Sets vec4 uniform at given location */
    void setUniform(int location, int count, const float* val) const;                   /**< Sets float array uniform at given location */
    void setUniform(int location, int count, const glm::vec2* val) const;               /**< Sets vec2 array uniform at given location */
    void setUniform(int location, int count, const glm::vec3* val) const;               /**< Sets vec3 array uniform at given location */
    void setUniform(int location, int count, const glm::vec4* val) const;               /**< Sets vec4 array uniform at given location */

    void setUniform(int location, int val) const;                                       /**< Sets int uniform at given location */
    void setUniform(int location, int val0, int val1) const;                            /**< Sets ivec2 uniform at given location */
    void setUniform(int location, const glm::ivec2& val) const;                         /**< Sets ivec2 uniform at given location */
    void setUniform(int location, int val0, int val1, int val2) const;                  /**< Sets ivec3 uniform at given location */
    void setUniform(int location, const glm::ivec3& val) const;                         /**< Sets ivec3 uniform at given location */
    void setUniform(int location, int val0, int val1, int val2, int val3) const;        /**< Sets ivec4 uniform at given location */
    void setUniform(int location, const glm::ivec4& val) const;                         /**< Sets ivec4 uniform at given location */
    void setUniform(int location, int count, const int* val) const;                     /**< Sets int array uniform at given location */
    void setUniform(int location, int count, const glm::ivec2* val) const;              /**< Sets ivec2 array uniform at given location */
    void setUniform(int location, int count, const glm::ivec3* val) const;              /**< Sets ivec3 array uniform at given location */
    void setUniform(int location, int count, const glm::ivec4* val) const;              /**< Sets ivec4 array uniform at given location */

    void setUniform(int location, unsigned int val) const;                              /**< Sets unsigned int uniform at given location */
    void setUniform(int location, unsigned int val0, unsigned int val1) const;          /**< Sets uvec2 uniform at given location */
    void setUniform(int location, const glm::uvec2& val) const;                         /**< Sets uvec2 uniform at given location */
    void setUniform(int location, unsigned int val0, unsigned int val1, unsigned int val2) const;/**< Sets uvec3 uniform at given location */
    void setUniform(int location, const glm::uvec3& val) const;                         /**< Sets uvec3 uniform at given location */
    void setUniform(int location, unsigned int val0, unsigned int val1, unsigned int val2, unsigned int val3) const;/**< Sets uvec4 uniform at given location */
    void setUniform(int location, const glm::uvec4& val) const;                         /**< Sets uvec4 uniform at given location */
    void setUniform(int location, int count, const unsigned int* val) const;            /**< Sets unsigned int array uniform at given location */
    void setUniform(int location, int count, const glm::uvec2* val) const;              /**< Sets uvec4 array uniform at given location */
    void setUniform(int location, int count, const glm::uvec3* val) const;              /**< Sets uvec4 array uniform at given location */
    void setUniform(int location, int count, const glm::uvec4* val) const;              /**< Sets uvec4 array uniform at given location */


    void setUniform(int location, int count, const glm::mat4* val) const;               /**< Sets mat4x4 uniform at given location */

    void setUniform(int location, TextureUnit unit) const;                              /**< Sets sampler uniform at given location */
    void setUniform(int location, ImageUnit unit) const;                                /**< Sets image uniform at given location */

private:

    ShaderProgramID m_id;

    static inline R::ShaderProgram* s_impl = nullptr;
};

}