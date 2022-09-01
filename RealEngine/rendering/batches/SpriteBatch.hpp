/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <RealEngine/rendering/output/Surface.hpp>
#include <RealEngine/rendering/textures/Texture.hpp>
#include <RealEngine/rendering/vertices/ShaderProgram.hpp>
#include <RealEngine/rendering/batches/Sprite.hpp>
#include <RealEngine/rendering/vertices/VertexArray.hpp>

namespace RE {

enum class GlyphSortType {
    NONE,
    NEG_TOP,
    POS_TOP,
    TEXTURE
};

class Glyph {
public:
    Glyph(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy texture, int depth, Color color);
    Glyph(const glm::vec4& posSize, const glm::vec4& uv, TextureProxy texture, int depth, Color color, float radAngle, const glm::vec2& origin = glm::vec2(0.0f, 0.0f));

    TextureProxy texture;
    int depth;

    VertexPOCOUV topLeft;
    VertexPOCOUV topRight;
    VertexPOCOUV botLeft;
    VertexPOCOUV botRight;
};

struct DrawBatch {
    DrawBatch(int offset, unsigned int count, TextureProxy texture) : offset(offset), count(count), texture(texture) {};

    int offset;
    int count;
    TextureProxy texture;
};

/**
 * @brief Draws sprites, surfaces and other textures
 * @tparam R The renderer that will perform the commands
*/
template<typename R = RendererLateBind>
class SpriteBatch {
public:

    /**
     * @brief Constructs new SpriteBatch
     * @param sources Sources used to construct the shader program that will be used for drawing
    */
    SpriteBatch(const ShaderProgramSources& sources);

    void begin();
    void end(GlyphSortType sortType);

    //UNCOLORED
    void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, TextureProxy texture, int depth);//Rotated to the right
    void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, TextureProxy texture, int depth, float radAngle, const glm::vec2& origin = glm::vec2(0.0f, 0.0f));//Rotated based on the angle
    void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, TextureProxy texture, int depth, const glm::vec2& direction, const glm::vec2& origin = glm::vec2(0.0f, 0.0f));//Rotated based on the vector
    //COLORED
    void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, TextureProxy texture, int depth, Color color);//Rotated to the right
    void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, TextureProxy texture, int depth, Color color, float radAngle, const glm::vec2& origin = glm::vec2(0.0f, 0.0f));//Rotated based on the angle
    void add(const glm::vec4& posSizeRectangle, const glm::vec4& uvRectagle, TextureProxy texture, int depth, Color color, const glm::vec2& direction, const glm::vec2& origin = glm::vec2(0.0f, 0.0f));//Rotated based on the vector

    //UNCOLORED, UNSTRETCHED
    void addTexture(const Texture* texture, const glm::vec2& position, int depth);//Unrotated
    void addTexture(const Texture* texture, const glm::vec2& position, int depth, float radAngle);//Rotated based on the angle
    void addTexture(const Texture* texture, const glm::vec2& position, int depth, const glm::vec2& direction);//Rotated based on the vector
    //COLORED, STRETCHED
    void addTexture(const Texture* texture, const glm::vec2& position, int depth, Color color, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Unrotated
    void addTexture(const Texture* texture, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Rotated based on the angle
    void addTexture(const Texture* texture, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale);//Rotated based on the vector

    //UNCOLORED, UNSTRETCHED
    void addSprite(const Sprite& sprite, const glm::vec2& position, int depth);//Unrotated
    void addSprite(const Sprite& sprite, const glm::vec2& position, int depth, float radAngle);//Rotated based on the angle
    void addSprite(const Sprite& sprite, const glm::vec2& position, int depth, const glm::vec2& direction);//Rotated based on the vector
    //COLORED, STRETCHED
    void addSprite(const Sprite& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Unrotated
    void addSprite(const Sprite& sprite, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Rotated based on the angle
    void addSprite(const Sprite& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale);//Rotated based on the vector

    //COLORED, STRETCHED BY FULLSPRITE
    void addSprite(const FullSprite& sprite, const glm::vec2& position, int depth);//Unrotated
    void addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, float radAngle);//Rotated based on the angle
    void addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, const glm::vec2& direction);//Rotated based on the vector
    //COLORED, STRETCHED BY USER
    void addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Unrotated
    void addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Rotated based on the angle
    void addSprite(const FullSprite& sprite, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& scale);//Rotated based on the vector

    //UNCOLORED, UNSTRETCHED
    void addSubimage(const Texture* texture, const glm::vec2& position, int depth, const glm::vec2& subImg_Spr);//Unrotated
    void addSubimage(const Texture* texture, const glm::vec2& position, int depth, float radAngle, const glm::vec2& subImg_Spr);//Rotated based on the angle
    void addSubimage(const Texture* texture, const glm::vec2& position, int depth, const glm::vec2& direction, const glm::vec2& subImg_Spr);//Rotated based on the vector
    //COLORED, STRETCHED
    void addSubimage(const Texture* texture, const glm::vec2& position, int depth, Color color, const glm::vec2& subImg_Spr, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Unrotated
    void addSubimage(const Texture* texture, const glm::vec2& position, int depth, Color color, float radAngle, const glm::vec2& subImg_Spr, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Rotated based on the angle
    void addSubimage(const Texture* texture, const glm::vec2& position, int depth, Color color, const glm::vec2& direction, const glm::vec2& subImg_Spr, const glm::vec2& scale);//Rotated based on the vector

    //UNCOLORED, UNSTRETCHED
    void addSurface(const Surface<R>& surface, const glm::vec2& position, int depth, int index);//Unrotated
    void addSurface(const Surface<R>& surface, const glm::vec2& position, int depth, int index, float radAngle);//Rotated based on the angle
    void addSurface(const Surface<R>& surface, const glm::vec2& position, int depth, int index, const glm::vec2& direction);//Rotated based on the vector
    //COLORED, STRETCHED
    void addSurface(const Surface<R>& surface, const glm::vec2& position, int depth, int index, Color color, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Unrotated
    void addSurface(const Surface<R>& surface, const glm::vec2& position, int depth, int index, Color color, float radAngle, const glm::vec2& scale = glm::vec2(1.0f, 1.0f));//Rotated based on the angle
    void addSurface(const Surface<R>& surface, const glm::vec2& position, int depth, int index, Color color, const glm::vec2& direction, const glm::vec2& scale);//Rotated based on the vector

    /**
     * @brief Draws the batch with stored shader program
    */
    void draw();

    /**
     * @brief Draws once with different shader program  (the shader is not stored)
     * @param program 
    */
    void draw(const ShaderProgram<R>& program);

    /**
     * @brief Switches to a different program that will be used for drawing
    */
    void switchShaderProgram(const ShaderProgramSources& sources);

private:

    void sortGlyphs(GlyphSortType sortType);
    void createDrawBatches();

    VertexArray<R> m_vao;
    Buffer<R> m_vbo{0, BufferAccessFrequency::STREAM, BufferAccessNature::DRAW};

    std::vector<Glyph*> m_glyphPointers;
    std::vector<Glyph> m_glyphs;
    std::vector<DrawBatch> m_drawBatches;
    std::vector<VertexPOCOUV> m_vertices;

    static bool compareNegToPos(Glyph* a, Glyph* b);
    static bool comparePosToNeg(Glyph* a, Glyph* b);
    static bool compareTexture(Glyph* a, Glyph* b);

    ShaderProgram<R> m_shaderProgram;

    static inline constexpr glm::vec4 UV_RECT = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    static inline constexpr glm::vec4 SUV_RECT = glm::vec4(0.0f, 1.0f, 1.0f, -1.0f);//Used for drawing surfaces
    static inline constexpr Color WHITE{255, 255, 255, 255};
};

}
