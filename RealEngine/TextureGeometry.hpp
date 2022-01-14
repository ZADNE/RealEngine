#pragma once
#include <glm/vec2.hpp>


namespace RE {

/**
 * @brief Texture geometry defines logical size and layout of texture.
 *
 * Texture geometry is typically independent of actual size of texture defined by its image.
 *
 * If not defined by image, texture geometry is a grid of subimages
 * where each image has same width and height. Paddings between subimages are not allowed.
 * All subimages also have same pivot. Pivot is a point which defines the center of the subimage
 * for transformations (such as rotation or scale).
 * Subimages placed along X axis form together a sprite.
 * There can also be multiple sprites in single geometry so these are placed under each other.
 *
 * @see TextureImage
*/
class TextureGeometry {
public:
	/**
	 * @brief Constructs geometry that is to be defined by image.
	 *
	 * All getters will return zeros for this as they lose relevance.
	*/
	TextureGeometry() : p_definedByImage(true) {}

	/**
	 * @brief Constructs geometry that has single subimage of given dimensions and pivot
	 * @param dims Dimension of the single subimage
	 * @param pivot Pivot of the subimage
	*/
	TextureGeometry(const glm::vec2& dims, const glm::vec2& pivot = {0.0f, 0.0f});

	/**
	 * @brief Contructs geometry as a grid of subimages
	 * @param subimageDims Dimension of the subimages (same for all subimages)
	 * @param pivot Pivot of the subimages (same for all subimages)
	 * @param subimagesSpritesCount x = number of subimages in sprite, y = number of sprites
	*/
	TextureGeometry(const glm::vec2& subimageDims, const glm::vec2& pivot, const glm::vec2& subimagesSpritesCount);

	/**
	 * @brief Gets whether geometry is to be defined by image (only default-constructed is)
	 * @return True if geometry is to be defined by image, false otherwise.
	*/
	bool isGeometryDefinedByImage() const { return p_definedByImage; }

	/**
	 * @brief Gets dimensions of a subimage (all subimages have same dimensions)
	 * @return Dimensions of subimage
	*/
	glm::vec2 getSubimageDims() const { return p_subimageDims; }

	/**
	 * @brief Gets pivot of a subimage (all subimages have same pivot)
	 *
	 * Pivot is a point which defines the center of the subimage for transformations (such as rotation or scale).
	 * @return Pivot of subimage
	*/
	glm::vec2 getPivot() const { return p_pivot; }

	/**
	 * @brief Gets the count of subimages in sprites (all sprites have same count of subimages) and count of sprites.
	 *
	 * Subimages placed along X axis form together a sprite.
	 * There can also be multiple sprites in single geometry so these are placed under each other.
	 * @return Count of subimages in sprite and count of sprites
	*/
	glm::vec2 getSubimagesSpritesCount() const { return p_subimagesSpritesCount; }
protected:
	glm::vec2 p_subimageDims{};			/**< Dimensions of subimages*/
	glm::vec2 p_pivot{};				/**< Pivot of subimages*/
	glm::vec2 p_subimagesSpritesCount{};/**< Count of subimages in sprite and count of sprites*/
	bool p_definedByImage = false;		/**< Flag that defines whether geometry is to be defined by image*/
};

}
