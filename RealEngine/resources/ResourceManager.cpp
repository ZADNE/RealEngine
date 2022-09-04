/*! 
 *  @author    Dubsky Tomas
 */
#include <RealEngine/resources/ResourceManager.hpp>

namespace RE {

template<RE::Renderer R>
TexturePtr ResourceManager<R>::texture(const std::string& filePath) {
    return s_textureCache.texture(filePath);
}

template ResourceManager<RendererLateBind>;
template ResourceManager<RendererGL46>;

}