/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/output/Surface.hpp>

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include <RealEngine/utility/error.hpp>
#include <RealEngine/rendering/output/Viewport.hpp>
#include <RealEngine/rendering/vertices/default_shaders.hpp>
#include <RealEngine/rendering/textures/TextureFlagsToString.hpp>
#include <RealEngine/resources/ResourceManager.hpp>
#include <RealEngine/rendering/capabilities.hpp>

#include <RealEngine/rendering/RendererLateBind.hpp>
#include <RealEngine/rendering/RendererGL46.hpp>


namespace RE {

template<typename R>
Surface<R>::Surface(const Raster& image, const TextureParameters& params, unsigned int numberOfTextures/* = 1*/, bool disableBlend/* = false*/, bool updateUniforms/* = true*/) :
    m_disableBlend(disableBlend),
    m_updateUniformBuffer(updateUniforms),
    m_params(params) {

    if (m_params.getMinFilterMipmapsUsage() == TextureMinFilterMipmapsUsage::YES) {
        error("Surface cannot have mipmaps");
        m_params.setMinFilter(TextureMinFilter::LINEAR_NO_MIPMAPS);//Mipmaps are not allowed for surfaces
    }

    resize(image, numberOfTextures);
}

template<typename R>
void Surface<R>::setTarget() const {
    m_framebuffer.targetMe(FramebufferTarget::DRAWING);

    Viewport::set(glm::ivec2(0, 0), m_textures[0].getTrueDims());

    if (m_disableBlend) {
        BlendingCapability<R>::disable();
    }

    if (m_updateUniformBuffer) {
        glm::vec2 trueDims = m_textures[0].getTrueDims();
        glm::mat4 matrix = glm::ortho(0.0f, trueDims.x, 0.0f, trueDims.y);
        Viewport::getWindowMatrixUniformBuffer().overwrite(0u, matrix);
    }
}

template<typename R>
void Surface<R>::resetTarget() const {
    DefaultFrameBuffer<R>::targetMe(FramebufferTarget::DRAWING);

    Viewport::setToWholeWindow();

    if (m_disableBlend) {
        BlendingCapability<R>::enable();
    }

    if (m_updateUniformBuffer) {
        Viewport::setWindowMatrixToMatchViewport();
    }
}

template<typename R>
void Surface<R>::associateTexturesWithOutputs(const std::vector<FramebufferOutput>& outputs) {
    m_framebuffer.associateAttachementsWithOutputs(outputs);
}

template<typename R>
void Surface<R>::resize(const Raster& image, unsigned int numberOfTextures) {
    //Delete framebuffe
    m_textures.clear();
    m_textures.reserve(numberOfTextures);
    for (size_t i = 0; i < numberOfTextures; i++) {
        m_textures.emplace_back(image, m_params);
    }
    if (numberOfTextures > 0) {
        attachTexturesToFramebuffer();
    }
}

template<typename R>
void Surface<R>::clear(const glm::vec4& color, int index) {
    m_textures[index].clear(color);
}

template<typename R>
void Surface<R>::clear(Color color, int index) {
    m_textures[index].clear(color);
}

template<typename R>
const Texture& Surface<R>::getTexture(int index) const {
    return m_textures[index];
}

template<typename R>
void Surface<R>::setPivot(const glm::vec2& pivot, int index) {
    m_textures[index].setPivot(pivot);
}

template<typename R>
void Surface<R>::setPivot(const glm::vec2& pivot) {
    for (auto& texture : m_textures) {
        texture.setPivot(pivot);
    }
}

template<typename R>
void Surface<R>::attachTexturesToFramebuffer() {
    std::vector<FramebufferOutput> fbOutputs;
    fbOutputs.reserve(m_textures.size());

    for (unsigned int i = 0; i < static_cast<unsigned int>(m_textures.size()); i++) {
        fbOutputs.push_back(FramebufferOutputN(i));
        m_framebuffer.attachImage(FramebufferAttachmentColorN(i), m_textures[i], 0);
    }

    if (m_framebuffer.checkTargetability(FramebufferTarget::DRAWING) != FramebufferTargetability::TARGETABLE) {
        fatalError("Failed to set up frame buffer!");
    }
}

template Surface<RendererLateBind>;
template Surface<RendererGL46>;

}