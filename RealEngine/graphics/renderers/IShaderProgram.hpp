/*!
 *  @author    Dubsky Tomas
 */
#pragma once

#include <RealEngine/graphics/ShaderProgram.hpp>

namespace RE {

/**
* @brief Is a renderer-agnostic interface to shader program implementation.
* 
* @see RE::ShaderProgram
*/
class IShaderProgram {
public:
	virtual void construct(ShaderProgram& sp) const = 0;
	virtual void destruct(ShaderProgram& sp) const = 0;


	virtual void use(ShaderProgram& sp) const = 0;
	virtual void unuse(ShaderProgram& sp) const = 0;

	virtual void dispatchCompute(ShaderProgram& sp, const glm::uvec3& groupCount, bool use) const = 0;
	virtual void dispatchCompute(ShaderProgram& sp, GLintptr indirect, bool use) const = 0;

	virtual void printInfo(ShaderProgram& sp) const = 0;
	virtual void backInterfaceBlock(ShaderProgram& sp, GLuint interfaceBlockIndex, const BufferTypedIndex& index) const = 0;


	virtual void setUniform(ShaderProgram& sp, int location, float val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, float val0, float val1) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, const glm::vec2& val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, float val0, float val1, float val2) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, const glm::vec3& val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, float val0, float val1, float val2, float val3) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, const glm::vec4& val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, int count, const float* val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, int count, const glm::vec2* val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, int count, const glm::vec3* val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, int count, const glm::vec4* val) const = 0;

	virtual void setUniform(ShaderProgram& sp, int location, GLint val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, GLint val0, GLint val1) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, const glm::ivec2& val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, GLint val0, GLint val1, GLint val2) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, const glm::ivec3& val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, GLint val0, GLint val1, GLint val2, GLint val3) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, const glm::ivec4& val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, int count, const GLint* val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, int count, const glm::ivec2* val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, int count, const glm::ivec3* val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, int count, const glm::ivec4* val) const = 0;

	virtual void setUniform(ShaderProgram& sp, int location, GLuint val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, GLuint val0, GLuint val1) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, const glm::uvec2& val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, GLuint val0, GLuint val1, GLuint val2) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, const glm::uvec3& val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, GLuint val0, GLuint val1, GLuint val2, GLuint val3) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, const glm::uvec4& val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, int count, const GLuint* val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, int count, const glm::uvec2* val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, int count, const glm::uvec3* val) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, int count, const glm::uvec4* val) const = 0;


	virtual void setUniform(ShaderProgram& sp, int location, int count, const glm::mat4* val) const = 0;

	virtual void setUniform(ShaderProgram& sp, int location, TextureUnit unit) const = 0;
	virtual void setUniform(ShaderProgram& sp, int location, ImageUnit unit) const = 0;

}