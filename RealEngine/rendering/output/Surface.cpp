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


namespace RE {

Surface::Surface(const Raster& image, const TextureParameters& params, unsigned int numberOfTextures/* = 1*/, bool disableBlend/* = false*/, bool updateUniforms/* = true*/) :
	m_disableBlend(disableBlend),
	m_updateUniformBuffer(updateUniforms),
	m_params(params) {

	if (m_params.getMinFilterMipmapsUsage() == TextureMinFilterMipmapsUsage::YES) {
		error("Surface cannot have mipmaps");
		m_params.setMinFilter(TextureMinFilter::LINEAR_NO_MIPMAPS);//Mipmaps are not allowed for surfaces
	}

	resize(image, numberOfTextures);
}

void Surface::setTarget() const {
	m_framebuffer.targetMe(FramebufferTarget::DRAWING);

	Viewport::set(glm::ivec2(0, 0), m_textures[0].getTrueDims());

	if (m_disableBlend) {
		glDisable(GL_BLEND);
	}

	if (m_updateUniformBuffer) {
		glm::vec2 trueDims = m_textures[0].getTrueDims();
		glm::mat4 matrix = glm::ortho(0.0f, trueDims.x, 0.0f, trueDims.y);
		Viewport::getWindowMatrixUniformBuffer().overwrite(0u, matrix);
	}
}

void Surface::resetTarget() const {
	DefaultFrameBuffer::targetMe(FramebufferTarget::DRAWING);

	Viewport::setToWholeWindow();

	if (m_disableBlend) {
		glEnable(GL_BLEND);
	}

	if (m_updateUniformBuffer) {
		Viewport::setWindowMatrixToMatchViewport();
	}
}

void Surface::associateTexturesWithOutputs(const std::vector<FramebufferOutput>& outputs) {
	m_framebuffer.associateAttachementsWithOutputs(outputs);
}

void Surface::resize(const Raster& image, unsigned int numberOfTextures) {
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

void Surface::clear(const glm::vec4& color, int index) {
	m_textures[index].clear(color);
}

void Surface::clear(Color color, int index) {
	m_textures[index].clear(color);
}

const Texture& Surface::getTexture(int index) const {
	return m_textures[index];
}

void Surface::setPivot(const glm::vec2& pivot, int index) {
	m_textures[index].setPivot(pivot);
}

void Surface::setPivot(const glm::vec2& pivot) {
	for (auto& texture : m_textures) {
		texture.setPivot(pivot);
	}
}

void Surface::attachTexturesToFramebuffer() {
	std::vector<FramebufferOutput> fbOutputs;
	fbOutputs.reserve(m_textures.size());

	for (size_t i = 0; i < m_textures.size(); i++) {
		fbOutputs.push_back(FramebufferOutputN(i));
		m_framebuffer.attachImage(FramebufferAttachmentColorN(i), m_textures[i], 0);
	}

	if (m_framebuffer.checkTargetability(FramebufferTarget::DRAWING) != FramebufferTargetability::TARGETABLE) {
		fatalError("Failed to set up frame buffer!");
	}
}

}