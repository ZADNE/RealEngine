/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <GL/glew.h>

namespace RE {

/**
 * @brief Represents a binding point that a texture can be bound to
*/
class TextureUnit {
	friend class Texture;
	friend class TextureProxy;
	friend class GL46_ShaderProgram;
public:
	TextureUnit(GLuint unit) : m_unit(unit) {}

	void setActive() const { glActiveTexture(GL_TEXTURE0 + m_unit); }
private:
	GLuint  m_unit;/**< Binding point name */
};

}