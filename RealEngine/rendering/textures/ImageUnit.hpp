/*!
 *  @author    Dubsky Tomas
 */
#pragma once

namespace RE {

/**
 * @brief Specifies how a bound image will be accessed
*/
enum class ImageAccess : GLenum {
	READ_ONLY = GL_READ_ONLY,
	WRITE_ONLY = GL_WRITE_ONLY,
	READ_WRITE = GL_READ_WRITE
};

/**
 * @brief Represents a binding point that an image from a texture can be bound to
*/
class ImageUnit {
	friend class Texture;
	friend class TextureProxy;
	friend class GL46_ShaderProgram;
	friend class GL46_Texture;
public:
	ImageUnit(GLuint unit) : m_unit(unit) {}
private:
	GLuint  m_unit;/**< Binding point name */
};

}