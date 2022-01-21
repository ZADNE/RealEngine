#pragma once
#include <vector>

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <RealEngine/graphics/Surface.hpp>
#include <RealEngine/graphics/texture/Texture.hpp>
#include <RealEngine/graphics/Vertex.hpp>
#include <RealEngine/graphics/Sprite.hpp>
#include <RealEngine/resources/ResourceManager.hpp>
#include <RealEngine/graphics/default_shaders.hpp>



namespace RE {

	enum class GlyphSortType {
		NONE,
		NEG_TOP,
		POS_TOP,
		TEXTURE
	};

	class Glyph {
	public:
		Glyph(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, GLuint Texture, int Depth, Colour colour);
		Glyph(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, GLuint Texture, int Depth, Colour colour, float radAngle, const glm::vec2& origin = glm::vec2(0.0f, 0.0f));

		GLuint texture;
		int depth;

		VertexPOCOUV topLeft;
		VertexPOCOUV topRight;
		VertexPOCOUV botLeft;
		VertexPOCOUV botRight;
	private:
		glm::vec2 rotatePoint(const glm::vec2& point, float radAngle) const;
	};

	class DrawBatch {
	public:
		DrawBatch(GLuint offset, GLuint numberVertices, GLuint texture) : m_offset(offset), m_numberVertices(numberVertices), m_texture(texture) {};
		GLuint m_offset;
		GLuint m_numberVertices;
		GLuint m_texture;
	private:

	};

	class SpriteBatch {
	public:
		SpriteBatch();
		~SpriteBatch();

		void begin(GlyphSortType sortType = GlyphSortType::TEXTURE);
		void end();

		//UNCOLORED
		void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, GLuint texture, int depth);//Rotated to the right
		void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, GLuint texture, int depth, float radAngle, const glm::vec2& origin = glm::vec2(0.0f, 0.0f));//Rotated based on the angle
		void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, GLuint texture, int depth, const glm::vec2& direction, const glm::vec2& origin = glm::vec2(0.0f, 0.0f));//Rotated based on the vector
		//COLORED
		void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, GLuint texture, int depth, Colour colour);//Rotated to the right
		void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, GLuint texture, int depth, Colour colour, float radAngle, const glm::vec2& origin = glm::vec2(0.0f, 0.0f));//Rotated based on the angle
		void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, GLuint texture, int depth, Colour colour, const glm::vec2& direction, const glm::vec2& origin = glm::vec2(0.0f, 0.0f));//Rotated based on the vector

		//UNCOLORED, UNSTRETCHED
		void addTexture(const Texture* texture, const glm::vec2& position, int depth);//Unrotated
		void addTexture(const Texture* texture, const glm::vec2& position, int depth, float radAngle);//Rotated based on the angle
		void addTexture(const Texture* texture, const glm::vec2& position, int depth, const glm::vec2& direction);//Rotated based on the vector
		//COLORED, STRETCHED
		void addTexture(const Texture* texture, const glm::vec2& position, int depth, Colour colour, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Unrotated
		void addTexture(const Texture* texture, const glm::vec2& position, int depth, Colour colour, float radAngle, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Rotated based on the angle
		void addTexture(const Texture* texture, const glm::vec2& position, int depth, Colour colour, const glm::vec2& direction, const glm::vec2& scale);//Rotated based on the vector

		//UNCOLORED, UNSTRETCHED
		void addSprite(const Sprite& sprite, const glm::vec2& position, int depth);//Unrotated
		void addSprite(const Sprite& sprite, const glm::vec2& position, int depth, float radAngle);//Rotated based on the angle
		void addSprite(const Sprite& sprite, const glm::vec2& position, int depth, const glm::vec2& direction);//Rotated based on the vector
		//COLORED, STRETCHED
		void addSprite(const Sprite& sprite, const glm::vec2& position, int depth, Colour colour, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Unrotated
		void addSprite(const Sprite& sprite, const glm::vec2& position, int depth, Colour colour, float radAngle, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Rotated based on the angle
		void addSprite(const Sprite& sprite, const glm::vec2& position, int depth, Colour colour, const glm::vec2& direction, const glm::vec2& scale);//Rotated based on the vector

		//COLORED, STRETCHED BY FULLSPRITE
		void addSprite(const FullSprite& sprite, const glm::vec2& position, int depth);//Unrotated
		void addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, float radAngle);//Rotated based on the angle
		void addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, const glm::vec2& direction);//Rotated based on the vector
		//COLORED, STRETCHED BY USER
		void addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, Colour colour, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Unrotated
		void addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, Colour colour, float radAngle, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Rotated based on the angle
		void addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, Colour colour, const glm::vec2& direction, const glm::vec2& scale);//Rotated based on the vector

		//UNCOLORED, UNSTRETCHED
		void addSubimage(const Texture* texture, const glm::vec2& position, int depth, const glm::vec2& subImg_Spr);//Unrotated
		void addSubimage(const Texture* texture, const glm::vec2& position, int depth, float radAngle, const glm::vec2& subImg_Spr);//Rotated based on the angle
		void addSubimage(const Texture* texture, const glm::vec2& position, int depth, const glm::vec2& direction, const glm::vec2& subImg_Spr);//Rotated based on the vector
		//COLORED, STRETCHED
		void addSubimage(const Texture* texture, const glm::vec2& position, int depth, Colour colour, const glm::vec2& subImg_Spr, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Unrotated
		void addSubimage(const Texture* texture, const glm::vec2& position, int depth, Colour colour, float radAngle, const glm::vec2& subImg_Spr, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Rotated based on the angle
		void addSubimage(const Texture* texture, const glm::vec2& position, int depth, Colour colour, const glm::vec2& direction, const glm::vec2& subImg_Spr, const glm::vec2& scale);//Rotated based on the vector

		//UNCOLORED, UNSTRETCHED
		void addSurface(const Surface& surface, const glm::vec2& position, int depth, int index);//Unrotated
		void addSurface(const Surface& surface, const glm::vec2& position, int depth, int index, float radAngle);//Rotated based on the angle
		void addSurface(const Surface& surface, const glm::vec2& position, int depth, int index, const glm::vec2& direction);//Rotated based on the vector
		//COLORED, STRETCHED
		void addSurface(const Surface& surface, const glm::vec2& position, int depth, int index, Colour colour, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Unrotated
		void addSurface(const Surface& surface, const glm::vec2& position, int depth, int index, Colour colour, float radAngle, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Rotated based on the angle
		void addSurface(const Surface& surface, const glm::vec2& position, int depth, int index, Colour colour, const glm::vec2& direction, const glm::vec2& scale);//Rotated based on the vector

		void draw();
		//Draws once with different shader - the change is not saved (use changeShader() for that)
		void draw(const ShaderProgram& program);

		//Default parameter shader = standard shader
		void changeShader(ShaderProgramSource source = ShaderProgramSource::stdSprite);

		static SpriteBatch& std() {
			static SpriteBatch std{};
			return std;
		}
	private:
		void createVertexArray();
		void sortGlyphs();
		void createDrawBatches();

		GlyphSortType m_glyphSortType;

		GLuint m_vbo = 0;
		GLuint m_vao = 0;

		std::vector<Glyph*> m_glyphPointers;
		std::vector<Glyph> m_glyphs;
		std::vector<DrawBatch> m_drawBatches;

		static bool compareNegToPos(Glyph* a, Glyph* b);
		static bool comparePosToNeg(Glyph* a, Glyph* b);
		static bool compareTexture(Glyph* a, Glyph* b);

		//Draw defaults
		ShaderProgramPtr m_shader = RE::RM::getShaderProgram(ShaderProgramSource::stdSprite);

		const glm::vec4 m_UVRectangle = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		const glm::vec4 m_SUVRectangle = glm::vec4(0.0f, 1.0f, 1.0f, -1.0f);//Used for drawing surfaces
		const Colour m_whiteColour{ 255, 255, 255, 255 };
	};

}
