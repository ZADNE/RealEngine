/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vector>

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <RealEngine/graphics/textures/Texture.hpp>
#include <RealEngine/graphics/Vertex.hpp>

namespace RE {

class SurfaceTargetTextures;

/**
* @brief Represents a canvas that can be drawn into.
* 
* A single surface can have several layers (= textures).
*/
class Surface {
public:

	/**
	 * @brief Constructs surface without any textures (these can be later added via resize())
	*/
	Surface(const TextureParameters& params, bool disableBlend = false, bool updateUniforms = true) :
		Surface(Raster{{1, 1}}, params, 0, disableBlend, updateUniforms) {};

	/**
	 * @brief Constructs surface
	 *
	 * @param image Image to initialize all textures with
	 * @param params Parameters of all the textures, note that mipmaps are not allowed for surface textures
	 * @param numberOfTextures Number of textures of the surface
	 * @param disableBlend If true, blending will be disabled when drawing to the surface.
	 * @param updateUniforms If true, standard GlobalMatrices uniform will be updated when drawing to the surface.
	*/
	Surface(const Raster& image, const TextureParameters& params, unsigned int numberOfTextures = 1, bool disableBlend = false, bool updateUniforms = true);

	Surface(const Surface& other) = delete;
	Surface(Surface&& other) noexcept;

	Surface& operator=(const Surface& other) = delete;
	Surface& operator=(Surface&& other) noexcept;


	/**
	 * @brief Destroys the surface and all its textures
	 */
	~Surface();

	/**
	 * @brief Sets further drawing to be done to this surface
	 */
	void setTarget() const;

	/**
	 * @brief Set further drawing to be done directly to window
	 */
	void resetTarget() const;

	/**
	 * @brief Allows disabling of drawing to some textures
	 *
	 * @param targetTextures Structure describing which texture should be drawn to
	 */
	void setTargetTextures(const SurfaceTargetTextures& targetTextures);

	/**
	 * @brief Resizes surface.
	 *
	 * That is: deletes all previous textures and then creates new textures
	 * with new image and parameters given in costructor.
	 *
	 * @param image Image to construct new textures with
	 * @param numberOfTextures Number of textures of the surface
	*/
	void resize(const Raster& image, unsigned int numberOfTextures);

	//color = normalized RGBA [0.0f; 1.0f]
	//index = which texture of the surface should be cleared
	void clear(const glm::vec4& color, int index);
	void clear(RE::Color color, int index);

	//Getters
	TextureProxy getTextureProxy(int index = 0) const { return TextureProxy{m_textures[index]}; }

	const Texture& getTexture(int index = 0) const;

	//All textures in the surface have same dimensions
	glm::uvec2 getDims() const { return m_textures[0].getTrueDims(); }
	//Note that each texture may have different pivot
	glm::vec2 getPivot(int index = 0) const { return m_textures[index].getPivot(); }

	//Sets pivot for a single texture (each texture may have different pivot)
	void setPivot(const glm::vec2& pivot, int index);
	//Sets every texture same pivot (but note that every texture may have different pivot)
	void setPivot(const glm::vec2& pivot);
private:
	std::vector<Texture> m_textures;
	GLuint m_frameBuffer = 0;
	bool m_disableBlend;
	bool m_updateUniformBuffer;
	TextureParameters m_params;

	void attachTexturesToFBO();

	glm::mat4 m_previousViewMatrix;
};

/**
* Class used for targeting only specific textures
*/
class SurfaceTargetTextures {
	friend Surface;
public:
	/**
	* Initializes all textures as untargetted
	*/
	SurfaceTargetTextures();

	/**
	* Sets texture at given offset to target given attachment
	*/
	SurfaceTargetTextures& targetTexture(unsigned int index, unsigned int attachment);

	/**
	* Sets all textures as untargetted
	*/
	void untargetAllTextures();
private:
	GLenum m_buffers[16];
};

}
