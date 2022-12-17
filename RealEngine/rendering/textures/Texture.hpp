/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <string>

#include <vulkan/vulkan.hpp>

#include <RealEngine/rendering/textures/Raster.hpp>
#include <RealEngine/rendering/textures/TextureUnit.hpp>
#include <RealEngine/rendering/textures/ImageUnit.hpp>


namespace RE {

/**
 * @brief Holds a non-owning handle to a Texture
 *
 * @warning Once the texture that it represents is destroyed, the proxy
 * becomes invalid and should not be used!
*/
class TextureProxy {
    friend class VK13Fixture;
public:

    /**
     * @brief Contructs proxy that represents the given texture
     * @param texture Texture to present
    */
    TextureProxy(const Texture& texture) {};

    /**
     * @brief Binds the texture that this proxy represents.
     *
     * Behaviour is undefined if the originating texture was destroyed.
    */
    void bind() const {};

    /**
     * @brief Binds the texture that this proxy represents to the given texture unit.
     *
     * Behaviour is undefined if the originating texture was destroyed.
     *
     * @param unit Texture unit to bind the texture to.
    */
    void bind(TextureUnit unit) const {};

    auto operator<=>(const TextureProxy&) const = default;

private:

    int m_id;
};

/**
 * @brief Manages conversion from filename to full path to texture
*/
struct TextureSeed {

    std::string toFullPath() const {
        return s_folder + file + s_extension;
    }

    const std::string& file; /**< Filepath without extension */

private:

    static inline std::string s_folder = "textures/";
    static inline std::string s_extension = ".png";
};

/**
 * @brief Represents one (or more) images
 *
 * Textures are always stored in GPU memory.
 *
 * @see Raster
 * @see TextureParameters
*/
class Texture {
    friend class VK13Fixture;
    friend class DescriptorSet;
public:

    static inline constexpr TextureFlags DEFAULT_FLAGS{TextureFlags::RGBA8_NU_NEAR_NEAR_EDGE};
    static inline constexpr Color DEFAULT_BORDER_COLOR{255, 20, 147, 255};

    /**
     * @brief Default parameters for texture use RGBA_NU_NEAR_NEAR_EDGE flags and geometry defined by raster
     * @see TextureFlags::RGBA_NU_NEAR_NEAR_EDGE
    */
    static inline constexpr TextureParameters DEFAULT_PARAMETERS{{}, DEFAULT_FLAGS, DEFAULT_BORDER_COLOR};

    /**
     * @brief Constructs texture from PNG
     * @param filePathPNG Path to the PNG
     * @param defParams Parameters that are used if reAl chunk could not be loaded.
     *
     * Parameters are either loaded from reAl chunk of PNG,
     * or if that cannot be done (reAl chunk is missing or it has bad format),
     * default parameters are used instead.
     *
     * @see DEFAULT_PARAMETERS
    */
    Texture(const std::string& filePathPNG, const TextureParameters& defParams);

    /**
     * @brief Constructs texture from seed
     *
     * TextureSeed is converted to full path which is used to load the texture
    */
    Texture(const TextureSeed& seed);

    /**
     * @brief Contructs texture from raster and parameters
     *
     * Default parameters are used if not specified.
     * @see DEFAULT_PARAMETERS
     *
     * @param raster Specifies true dimensions of the texture and initial contents of the texture if pixels are provided
     * @param params Specifies how the texture will be drawn.
    */
    Texture(const Raster& raster, const TextureParameters& params = DEFAULT_PARAMETERS);

    Texture(const Texture&) = delete;
    Texture(Texture&& other) noexcept;

    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&& other) noexcept;

    /**
     * @brief Destroys the texture
    */
    ~Texture();

    TextureParameters getParameters() const;
    glm::vec2 getSubimageDims() const { return m_subimageDims; }
    glm::vec2 getPivot() const { return m_pivot; }
    glm::vec2 getSubimagesSpritesCount() const { return m_subimagesSpritesCount; }

    /**
     * @brief Saves level 0 as a PNG file.
     * @param filePathPNG Filepath to save, existing file will be overwritten withot warnings.
     * @return True if saved succesfully, false otherwise.
    */
    bool saveToFile(const std::string& filePathPNG);

    /**
     * @brief Saves texture as a PNG file with parameters different than current ones.
     * @param filePathPNG Filepath to save, existing file will be overwritten withot warnings.
     * @param TextureParameters Parameters to save (this does not override in-memory parameters of the texture)
     * @return True if saved succesfully, false otherwise.
    */
    bool saveToFile(const std::string& filePathPNG, const TextureParameters& params);

    const vk::Image& image() const { return m_image; }

private:

    void init(const Raster& raster, const TextureParameters& params);

    vk::DeviceMemory m_deviceMemory{};
    vk::Image m_image{};
    vk::ImageView m_imageView{};
    vk::Sampler m_sampler{};

    TextureFlags m_flags{};     /**< Flags associated with the texture */

    glm::vec2 m_subimageDims{}; /**< Dimensions of the texture */
    glm::vec2 m_pivot{};        /**< Pivot of the texture */
    glm::vec2 m_subimagesSpritesCount{};/**< Number of subimages and sprites of the texture */

    glm::uvec2 m_trueDims{};    /**< True dimensions of the texture */

    Color m_borderColor{};      /**< Border color of the texture */

    void transitionImageLayout(const vk::CommandBuffer& commandBuffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

    //Static members \|/

    static inline const vk::PhysicalDevice* s_physicalDevice = nullptr;
    static inline const vk::Device* s_device = nullptr;
    static inline const vk::CommandBuffer* s_commandBuffer = nullptr;

    static uint32_t selectMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
};

}
