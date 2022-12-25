/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>

#include <glm/vec2.hpp>

#include <RealEngine/resources/PNGLoader.hpp>
#include <RealEngine/rendering/textures/Texture.hpp>
#include <RealEngine/rendering/textures/TextureShape.hpp>


namespace RE {

/**
 * @brief Converts simple filename to full path to a texture
*/
struct TextureSeed {

    std::string toFullPath() const {
        return s_folder + file + s_fileExtension;
    }

    const std::string& file; /**< Filepath without extension */

private:

    static inline std::string s_folder = "textures/";
    static inline std::string s_fileExtension = ".png";
};

/**
 * @brief Is a 2D texture with associated shape
*/
class TextureShaped : public Texture {
public:

    /**
     * @brief   Constructs texture from PNG
     * @param   filePathPNG Path to the PNG
     * @throws  Throws when the PNG cannot be loaded
     * @details Shape of the texture is also loaded from the chunk,
     *          or if that cannot be done, default shape is used instead.
    */
    TextureShaped(const std::string& filePathPNG) :
        TextureShaped(PNGLoader::load(filePathPNG)) {}

    /**
     * @brief   Constructs texture from seed
     * @details TextureSeed is converted to full path which is used to load the texture
    */
    TextureShaped(const TextureSeed& seed) :
        TextureShaped(seed.toFullPath()) {}

    TextureShaped(const TextureShaped&) = delete;
    TextureShaped(TextureShaped&& other) noexcept;

    TextureShaped& operator=(const TextureShaped&) = delete;
    TextureShaped& operator=(TextureShaped&& other) noexcept;

    glm::vec2 subimageDims() const { return m_shape.subimageDims; }
    glm::vec2 pivot() const { return m_shape.pivot; }
    glm::vec2 subimagesSpritesCount() const { return m_shape.subimagesSpritesCount; }

    void setSubimageDims(const glm::vec2& subimageDims) { m_shape.subimageDims = subimageDims; }
    void setPivot(const glm::vec2& pivot) { m_shape.pivot = pivot; }
    void setSubimagesSpritesCount(const glm::vec2& subimagesSpritesCount) { m_shape.subimagesSpritesCount = subimagesSpritesCount; }

    /**
     * @brief   Saves level 0 of the texture as a PNG file
     * @param   filePathPNG Filepath to save, existing file will be overwritten without warnings
     * @details Also encodes its shape.
     * @return  True if saved succesfully, false otherwise
    */
    bool saveToFile(const std::string& filePathPNG);

private:

    TextureShaped(PNGLoader::PNGData&& pngData);

    TextureShape m_shape{};
};
}
