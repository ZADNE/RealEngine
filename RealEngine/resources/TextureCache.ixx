/*! 
 *  @author    Dubsky Tomas
 */
export module RealEngine.resources.TextureCache;
import std;
import RealEngine.rendering.textures.Texture;


export namespace RE {

/**
 * @brief Is a Texture that can be accessed with late-bind or as specific renderer
*/
union TextureUnion {
public:

    TextureUnion(const std::string& filePathPNG);

    ~TextureUnion();

    TextureUnion(const TextureUnion&) = delete;
    TextureUnion& operator=(const TextureUnion&) = delete;

    template<Renderer R = RendererLateBind>
    const Texture<R>& get() const {
        return m_lateBindTex;
    }

    template<>
    const Texture<RendererGL46>& get<RendererGL46>() const {
        return m_gl46Tex;
    }

private:

    Texture<RendererLateBind> m_lateBindTex;
    Texture<RendererGL46> m_gl46Tex;

    static_assert(sizeof(m_lateBindTex) == sizeof(m_gl46Tex));
};

/**
 * @brief Is a Texture that can be shared across late-bind and renderer-specific Rooms
*/
using SharedTexture = std::shared_ptr<TextureUnion>;

class TextureCache {
public:

    TextureCache() {}

    /**
     * @brief Gets texture as a shared resource.
     * @param filePathPNG Path to the PNG
     * @return Texture as a shared resource
    */
    SharedTexture texture(const std::string& filePathPNG);

private:

    std::unordered_map<std::string, std::weak_ptr<TextureUnion>> m_textureMap;
};

}