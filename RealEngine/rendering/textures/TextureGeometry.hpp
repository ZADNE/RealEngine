/*! 
 *  @author    Dubsky Tomas
 */
#pragma once
#include <glm/vec2.hpp>


namespace RE {

/**
 * @brief Defines logical size and layout of a texture.
 *
 * Texture geometry is typically independent of actual size of texture defined by its raster.
 *
 * If not defined by raster, texture geometry is a grid of subimages
 * where each raster has same width and height. Paddings between subimages are not allowed.
 * All subimages also have same pivot. Pivot is a point which defines the center of the subimage
 * for transformations (such as rotation or scale).
 * Subimages placed along X axis form together a sprite.
 * There can also be multiple sprites in single geometry so these are placed under each other.
 *
 * @see Raster
*/
class TextureGeometry {
public:
    /**
     * @brief Constructs geometry that is to be defined by raster.
     *
     * All getters will return zeros for this as they lose relevance.
    */
    constexpr TextureGeometry() : m_definedByRaster(true) {}

    /**
     * @brief Constructs geometry that has single subimage of given dimensions and pivot
     * @param dims Dimension of the single subimage
     * @param pivot Pivot of the subimage
    */
    constexpr TextureGeometry(const glm::vec2& dims, const glm::vec2& pivot = {0.0f, 0.0f}) :
        m_subimageDims(dims), m_pivot(pivot), m_subimagesSpritesCount(1.0f, 1.0f) {}

    /**
     * @brief Contructs geometry as a grid of subimages
     * @param subimageDims Dimension of the subimages (same for all subimages)
     * @param pivot Pivot of the subimages (same for all subimages)
     * @param subimagesSpritesCount x = number of subimages in sprite, y = number of sprites
    */
    constexpr TextureGeometry(const glm::vec2& subimageDims, const glm::vec2& pivot, const glm::vec2& subimagesSpritesCount) :
        m_subimageDims(subimageDims), m_pivot(pivot), m_subimagesSpritesCount(subimagesSpritesCount) {}

    /**
     * @brief Gets whether geometry is to be defined by image (only default-constructed is)
     * @return True if geometry is to be defined by image, false otherwise.
    */
    bool isGeometryDefinedByImage() const { return m_definedByRaster; }

    /**
     * @brief Gets dimensions of a subimage (all subimages have same dimensions)
     * @return Dimensions of subimage
    */
    glm::vec2 getSubimageDims() const { return m_subimageDims; }

    /**
     * @brief Gets pivot of a subimage (all subimages have same pivot)
     *
     * Pivot is a point which defines the center of the subimage for transformations (such as rotation or scale).
     * @return Pivot of subimage
    */
    glm::vec2 getPivot() const { return m_pivot; }

    /**
     * @brief Gets the count of subimages in sprites (all sprites have same count of subimages) and count of sprites.
     *
     * Subimages placed along X axis form together a sprite.
     * There can also be multiple sprites in single geometry so these are placed under each other.
     * @return Count of subimages in sprite and count of sprites
    */
    glm::vec2 getSubimagesSpritesCount() const { return m_subimagesSpritesCount; }
protected:
    glm::vec2 m_subimageDims{};         /**< Dimensions of subimages*/
    glm::vec2 m_pivot{};                /**< Pivot of subimages*/
    glm::vec2 m_subimagesSpritesCount{};/**< Count of subimages in sprite and count of sprites*/
    bool m_definedByRaster = false;     /**< Flag that defines whether geometry is to be defined by raster*/
};

}
