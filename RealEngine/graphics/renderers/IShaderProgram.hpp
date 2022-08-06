/*!
 *  @author    Dubsky Tomas
 */
#pragma once

#include <string>

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <RealEngine/graphics/buffers/Buffer.hpp>
#include <RealEngine/graphics/textures/TextureUnit.hpp>
#include <RealEngine/graphics/textures/ImageUnit.hpp>

namespace RE {

class ShaderSources;
struct ShaderProgramSources;
class ShaderProgram;

/**
* @brief Is a renderer-agnostic interface to shader program implementation.
*
* This is used internally by ShaderProgram (it does not implement this interface directly).
*
* @see RE::ShaderProgram
*/
class IShaderProgram {
public:
	virtual void construct(ShaderProgram& sp, const ShaderProgramSources& sources) const = 0;
	virtual void destruct(ShaderProgram& sp) const = 0;


	virtual void use(const ShaderProgram& sp) const = 0;
	virtual void unuse(const ShaderProgram& sp) const = 0;

	virtual void dispatchCompute(const ShaderProgram& sp, const glm::uvec3& groupCount, bool use) const = 0;
	virtual void dispatchCompute(const ShaderProgram& sp, GLintptr indirect, bool use) const = 0;

	virtual void printInfo(const ShaderProgram& sp) const = 0;
	virtual void backInterfaceBlock(const ShaderProgram& sp, GLuint interfaceBlockIndex, const BufferTypedIndex& index) const = 0;

	virtual int getUniformLocation(const ShaderProgram& sp, const std::string& name) const = 0;

	virtual void setUniform(const ShaderProgram& sp, int location, float val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, float val0, float val1) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, const glm::vec2& val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, float val0, float val1, float val2) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, const glm::vec3& val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, float val0, float val1, float val2, float val3) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, const glm::vec4& val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const float* val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const glm::vec2* val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const glm::vec3* val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const glm::vec4* val) const = 0;

	virtual void setUniform(const ShaderProgram& sp, int location, GLint val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, GLint val0, GLint val1) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, const glm::ivec2& val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, GLint val0, GLint val1, GLint val2) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, const glm::ivec3& val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, GLint val0, GLint val1, GLint val2, GLint val3) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, const glm::ivec4& val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const GLint* val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const glm::ivec2* val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const glm::ivec3* val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const glm::ivec4* val) const = 0;

	virtual void setUniform(const ShaderProgram& sp, int location, GLuint val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, GLuint val0, GLuint val1) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, const glm::uvec2& val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, GLuint val0, GLuint val1, GLuint val2) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, const glm::uvec3& val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, GLuint val0, GLuint val1, GLuint val2, GLuint val3) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, const glm::uvec4& val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const GLuint* val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const glm::uvec2* val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const glm::uvec3* val) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, int count, const glm::uvec4* val) const = 0;


	virtual void setUniform(const ShaderProgram& sp, int location, int count, const glm::mat4* val) const = 0;

	virtual void setUniform(const ShaderProgram& sp, int location, TextureUnit unit) const = 0;
	virtual void setUniform(const ShaderProgram& sp, int location, ImageUnit unit) const = 0;

};

}
