/*!
 *  @author    Dubsky Tomas
 */
#pragma once

namespace RE {

/**
 * @brief Represents a binding point that a texture can be bound to
*/
class TextureUnit {
	friend class Texture;
	friend class TextureProxy;
	friend class GL46_ShaderProgram;
	friend class GL46_Texture;
public:

	TextureUnit(GLuint unit) : m_unit(unit) {}

private:
	GLuint  m_unit;/**< Binding point name */
};

}