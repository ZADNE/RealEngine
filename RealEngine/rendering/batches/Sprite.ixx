/*!
 *  @author    Dubsky Tomas
 */
export module RealEngine.rendering.batches.Sprite;
import RealEngine.rendering.textures.Texture;
import RealEngine.rendering.vertices.Vertex;
export import RealEngine.rendering.Renderer;


export namespace RE {

/**
 * @brief Represents a static subimage within a texture.
 * @tparam R The renderer that will perform the commands
*/
template<Renderer R = RendererLateBind>
class SpriteStatic {
public:

    /**
     * @warning Sprite does not hold ownership of the Texture!
    */
    SpriteStatic(const Texture<R>& tex, float sprite, float subimage = 0.0f);

    virtual float getSpeed() const;
    virtual Color getColor() const;
    virtual glm::vec2 getScale() const;
    virtual glm::vec2 getSubimageSprite() const;

    virtual void step();

    const Texture<R>& getTexture() const { return *m_tex; };

protected:

    glm::vec2 m_subimageSprite;//X = subimage of the sprite, Y = sprite of the texture
    const Texture<R>* m_tex;
};

/**
 * @brief Represents an animated subimage within a texture.
 * @tparam R The renderer that will perform the commands
*/
template<Renderer R = RendererLateBind>
class SpriteAnimated : public SpriteStatic<R> {
public:

    SpriteAnimated(const Texture<R>& tex, float sprite, float subimage = 0.0f, float imageSpeed = 1.0f);

    void step() override;

    void setSpeed(float newSpeed);

    float getSpeed() const override;

protected:

    float m_imageSpeed;//Speed is added each beginStep to the current image position
};

/**
 * @brief Represents an animated subimage within a texture that can be tinted or scaled.
 * @tparam R The renderer that will perform the commands
*/
template<Renderer R = RendererLateBind>
class SpriteComplex : public SpriteAnimated<R> {
public:

    SpriteComplex(const Texture<R>& tex, float sprite, float subimage, float imageSpeed, Color color, const glm::vec2& scale);

    void setColor(Color color);
    void setScale(const glm::vec2& scale);

    Color getColor() const override;
    glm::vec2 getScale() const override;

protected:

    glm::vec2 m_scale;
    Color m_color;
};

}
