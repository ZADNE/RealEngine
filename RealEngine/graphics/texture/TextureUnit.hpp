#pragma once
#include <GL/glew.h>

namespace RE {

struct TextureUnit {
	TextureUnit(GLuint index) : m_index(index) {}

	void setActive() const { glActiveTexture(GL_TEXTURE0 + m_index); }
	GLint index() const { return m_index; }
private:
	GLint m_index;
};

}