/*!
 *  @author    Dubsky Tomas
 */
#include <RealEngine/rendering/output/Surface.hpp>

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include <RealEngine/main/Error.hpp>
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
		log("Surface cannot have mipmaps");
		m_params.setMinFilter(TextureMinFilter::LINEAR_NO_MIPMAPS);//Mipmaps are not allowed for surfaces
	}

	assert(numberOfTextures <= 16);

#if defined(_DEBUG) && defined(RE_LOG_SURFACE_CREATED)
	if (numberOfTextures > 0) std::cout << "{\n";
#endif // defined(_DEBUG) && defined(RE_LOG_SURFACE_CREATED)

	glCreateFramebuffers(1, &m_frameBuffer);

	resize(image, numberOfTextures);

#if defined(_DEBUG) && defined(RE_LOG_SURFACE_CREATED)
	std::cout << "Created surface (ID: " << m_frameBuffer << ")";
	std::cout << (numberOfTextures > 0 ? "}\n" : "\n");
#endif // defined(_DEBUG) && defined(RE_LOG_SURFACE_CREATED)
}

Surface::Surface(Surface&& other) noexcept :
	m_frameBuffer(other.m_frameBuffer),
	m_params(other.m_params),
	m_disableBlend(other.m_disableBlend),
	m_updateUniformBuffer(other.m_updateUniformBuffer) {
	other.m_frameBuffer = 0;
	std::swap(m_textures, other.m_textures);
}

Surface& Surface::operator=(Surface&& other) noexcept {
	std::swap(m_frameBuffer, other.m_frameBuffer);
	std::swap(m_textures, other.m_textures);
	m_disableBlend = other.m_disableBlend;
	m_updateUniformBuffer = other.m_updateUniformBuffer;
	m_params = other.m_params;
	return *this;
}

Surface::~Surface() {
#if defined(_DEBUG) && defined(RE_LOG_SURFACE_DESTROYED)
	auto numberOfTextures = m_textures.size();
	if (numberOfTextures > 0) std::cout << "{\n";
	m_textures.clear();
#endif // defined(_DEBUG) && defined(RE_LOG_SURFACE_DESTROYED)
	//Delete framebuffer
	if (m_frameBuffer != 0) {
		glDeleteFramebuffers(1, &m_frameBuffer);
	}
#if defined(_DEBUG) && defined(RE_LOG_SURFACE_DESTROYED)
	std::cout << "Destroyed surface (ID: " << m_frameBuffer << ")";
	std::cout << (numberOfTextures > 0 ? "}\n" : "\n");
#endif // defined(_DEBUG) && defined(RE_LOG_SURFACE_DESTROYED)
}

void Surface::setTarget() const {
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

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
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Viewport::setToWholeWindow();

	if (m_disableBlend) {
		glEnable(GL_BLEND);
	}

	if (m_updateUniformBuffer) {
		Viewport::setWindowMatrixToMatchViewport();
	}
}

void Surface::setTargetTextures(const SurfaceTargetTextures& targetTextures) {
	glNamedFramebufferDrawBuffers(m_frameBuffer, (GLsizei)m_textures.size(), targetTextures.m_buffers);
}

void Surface::resize(const Raster& image, unsigned int numberOfTextures) {
	//Delete framebuffe
	m_textures.clear();
	m_textures.reserve(numberOfTextures);
	for (size_t i = 0; i < numberOfTextures; i++) {
		m_textures.emplace_back(image, m_params);
	}
	if (numberOfTextures > 0) {
		attachTexturesToFBO();
	}
#if defined(_DEBUG) && defined(RE_LOG_SURFACE_RESIZED)
	std::cout << "Resized surface (ID: " << m_frameBuffer << ", ";
	if (m_textures.size() > 0) {
		std::cout << m_textures.size() << " textures@"
			<< m_textures[0].getTrueDims().x << "x" << m_textures[0].getTrueDims().y << "x" << to_string(m_textures[0].getChannels());
	} else {
		std::cout << "0 textures";
	}
	std::cout << ")\n";
#endif // defined(_DEBUG) && defined(RE_LOG_SURFACE_RESIZED)
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

void Surface::attachTexturesToFBO() {
	GLenum buffers[16];

	for (GLenum i = 0; i < (GLenum)m_textures.size(); i++) {
		buffers[i] = GL_COLOR_ATTACHMENT0 + i;
		//glNamedFramebufferTexture(m_frameBuffer, buffers[i], m_textures[i].getID(), 0);
	}

	glNamedFramebufferDrawBuffers(m_frameBuffer, (GLsizei)m_textures.size(), buffers);

	if (glCheckNamedFramebufferStatus(m_frameBuffer, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		fatalError("Failed to create surface frame buffer!");
	}
}

SurfaceTargetTextures::SurfaceTargetTextures() :
	m_buffers{
		GL_NONE, GL_NONE, GL_NONE, GL_NONE,
		GL_NONE, GL_NONE, GL_NONE, GL_NONE,
		GL_NONE, GL_NONE, GL_NONE, GL_NONE,
		GL_NONE, GL_NONE, GL_NONE, GL_NONE
} {

}

SurfaceTargetTextures& SurfaceTargetTextures::targetTexture(unsigned int index, unsigned int attachment) {
	assert(index < sizeof(m_buffers) / sizeof(m_buffers[0]));
	m_buffers[index] = GL_COLOR_ATTACHMENT0 + attachment;
	return *this;
}

void SurfaceTargetTextures::untargetAllTextures() {
	for (size_t i = 0; i < sizeof(m_buffers) / sizeof(m_buffers[0]); i++) {
		m_buffers[i] = GL_NONE;
	}
}

}