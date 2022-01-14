#pragma once
#include <GL\glew.h>
#include <glm\vec2.hpp>

namespace RE {

	const GLuint PRIMITIVE_RESTART_INDEX = 4294967295u;

	const unsigned int ATTLOC_PO = 0u;
	const unsigned int ATTLOC_CO = 1u;
	const unsigned int ATTLOC_UV = 2u;

	struct Position {
		Position() {}
		Position(GLfloat X, GLfloat Y): x(X), y(Y) {}
		Position(const glm::vec2& pos) : x(pos.x), y(pos.y) {}

		GLfloat x;
		GLfloat y;
	};

	struct Colour {
		Colour() {}
		Colour(GLubyte r, GLubyte g, GLubyte b, GLubyte a) : R(r), G(g), B(b), A(a) {}
		
		void setColour(GLubyte r, GLubyte g, GLubyte b, GLubyte a) {
			R = r;
			G = g;
			B = b;
			A = a;
		}

		GLubyte R;
		GLubyte G;
		GLubyte B;
		GLubyte A;
	};

	struct UV {
		UV() {}
		UV(GLfloat U, GLfloat V): u(U), v(V) {}
		UV(const glm::vec2& uv) : u(uv.x), v(uv.y) {}

		GLfloat u;
		GLfloat v;
	};

	struct VertexPOCOUV {
		VertexPOCOUV() {}
		VertexPOCOUV(Position pos, Colour col, UV uv) : position(pos), colour(col), uv(uv) {}

		void setPosition(GLfloat x, GLfloat y) {
			position.x = x;
			position.y = y;
		}

		void setColour(GLubyte r, GLubyte g, GLubyte b, GLubyte a) {
			colour.R = r;
			colour.G = g;
			colour.B = b;
			colour.A = a;
		}

		void setUV(GLfloat u, GLfloat v) {
			uv.u = u;
			uv.v = v;
		}

		Position position;
		Colour colour;
		UV uv;
	};

	struct VertexPOCO {
		VertexPOCO() {}
		VertexPOCO(Position pos, Colour col) : position(pos), colour(col) {}

		void setPosition(GLfloat x, GLfloat y) {
			position.x = x;
			position.y = y;
		}

		void setColour(GLubyte r, GLubyte g, GLubyte b, GLubyte a) {
			colour.R = r;
			colour.G = g;
			colour.B = b;
			colour.A = a;
		}

		Position position;
		Colour colour;
	};

	struct VertexPOUV {
		VertexPOUV() {}
		VertexPOUV(GLfloat x, GLfloat y, GLfloat u, GLfloat v): position(x, y), uv(u, v){ };

		Position position;
		UV uv;

		void setPosition(GLfloat x, GLfloat y) {
			position.x = x;
			position.y = y;
		}

		void setPosition(const glm::vec2& pos) {
			position.x = pos.x;
			position.y = pos.y;
		};

		void setUV(GLfloat u, GLfloat v) {
			uv.u = u;
			uv.v = v;
		}

		void setUV(const glm::vec2& uV) {
			uv.u = uV.x;
			uv.v = uV.y;
		}
	};

	struct VertexPO {
		VertexPO() {}
		VertexPO(GLfloat x, GLfloat y) {
			position.x = x;
			position.y = y;
		};

		VertexPO(Position pos): position(pos) {}


		void setPosition(GLfloat x, GLfloat y) {
			position.x = x;
			position.y = y;
		}

		void setPosition(const glm::vec2& pos) {
			position.x = pos.x;
			position.y = pos.y;
		};

		Position position;
	};

}
