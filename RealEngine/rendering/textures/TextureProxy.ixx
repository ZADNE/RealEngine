/*!
 *  @author    Dubsky Tomas
 */
export module RealEngine.rendering.textures.TextureProxy;
import std;
export import RealEngine.rendering.internal_interfaces.ITexture;
export import RealEngine.rendering.textures.Texture;
export import RealEngine.rendering.Renderer;


export namespace RE {

/**
 * @brief Holds a non-owning handle to a Texture
 * @tparam R The renderer that will perform the commands
 *
 * @warning Once the texture that it represents is destroyed, the proxy
 * becomes invalid and should not be used!
*/
template<Renderer R = RendererLateBind>
class TextureProxy {
    friend class GL46Fixture;
    friend class GL46Texture;
public:

    /**
     * @brief Contructs proxy that represents the given texture
     * @param texture Texture to present
    */
    TextureProxy(const Texture<R>& texture) :
        m_id(texture.m_id) {
    }

    /**
     * @brief Binds the texture that this proxy represents.
     *
     * Behaviour is undefined if the originating texture was destroyed.
    */
    void bind() const {
        s_impl->bind(m_id);
    }

    /**
     * @brief Binds the texture that this proxy represents to the given texture unit.
     *
     * Behaviour is undefined if the originating texture was destroyed.
     *
     * @param unit Texture unit to bind the texture to.
    */
    void bind(TextureUnit unit) const {
        s_impl->bind(m_id, unit);
    }

    auto operator<=>(const TextureProxy<R>&) const = default;

private:

    TextureID m_id;

    static inline R::Texture* s_impl = nullptr;
};

}
