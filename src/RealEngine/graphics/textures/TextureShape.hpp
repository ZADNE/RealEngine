/**
 *  @author    Dubsky Tomas
 */
#pragma once
#include <glm/vec2.hpp>

namespace re {

/**
 * @brief Defines logical size and layout of a shaped texture.
 *
 * Texture shape is typically independent of actual size of texture defined by
 * its texels.
 *
 * Texture shape is a grid of subimages
 * where each raster has same width and height. Paddings between subimages are
 * not allowed. All subimages also have same pivot. Pivot is a point which
 * defines the center of the subimage for transformations (such as rotation or
 * scale). Subimages placed along X axis form together a sprite. There can also
 * be multiple sprites in single shape so these are placed under each other.
 */
struct TextureShape {

    /**
     * @brief   Dimensions of subimages
     * @details All subimages have the same dimensions
     */
    glm::vec2 subimageDims = glm::vec2{0.0f, 0.0f};

    /**
     * @brief   Pivot of the texture
     * @details All subimages have the same pivot
     *          Pivot is the center point of the subimage for transformations
     * (such as rotation or scale)
     */
    glm::vec2 pivot = glm::vec2{0.0f, 0.0f};

    /**
     * @brief   Count of subimages and sprites
     * @details All sprites have same number of subimages
     *          Subimages placed along X axis form together a sprite.
     *          There can also be multiple sprites in single geometry so these
     * are placed under each other. In other words: x = number of subimages in
     * sprite, y = number of sprites
     */
    glm::vec2 subimagesSpritesCount = glm::vec2{1.0f, 1.0f};
};

} // namespace re
