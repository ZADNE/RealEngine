#pragma once
#include <GL/glew.h>

namespace RE {

class TextureUnit {
	friend class ShaderProgram;
	friend class Texture;
	friend class TextureProxy;
public:
	TextureUnit(GLuint index) : m_index(index) {}

	void setActive() const { glActiveTexture(GL_TEXTURE0 + m_index); }
private:
	GLint m_index;
};

}