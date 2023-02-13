﻿/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <glm/vec3.hpp>

#include <RealEngine/renderer/VulkanObject.hpp>


namespace RE {

/**
 * @brief Specifies parameters for texture creation
*/
struct TextureCreateInfo {
    //Image-related
    vk::ImageType type = vk::ImageType::e2D;
    vk::Format format = vk::Format::eR8G8B8A8Unorm;
    glm::uvec3 extent{};
    uint32_t layers = 1u;
    vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eSampled;
    vk::ImageLayout initialLayout = vk::ImageLayout::eReadOnlyOptimal;
    //Memory-related
    vk::MemoryPropertyFlagBits memory = vk::MemoryPropertyFlagBits::eDeviceLocal;
    //Sampler-related
    vk::Filter magFilter = vk::Filter::eNearest;
    vk::Filter minFilter = vk::Filter::eNearest;
    vk::SamplerMipmapMode mipmapMode = vk::SamplerMipmapMode::eNearest;
    //Raster-related
    std::vector<unsigned char> texels;
};

/**
 * @brief Represents one (or more) images
*/
class Texture: public VulkanObject {
public:

    /**
     * @brief Constructs Texture
     * @param createInfo Specifies parameters of the texture
    */
    Texture(const TextureCreateInfo& createInfo);

    Texture(const Texture&) = delete;                           /**< Noncopyable */
    Texture& operator=(const Texture&) = delete;                /**< Noncopyable */

    Texture(Texture&& other) noexcept;                          /**< Movable */
    Texture& operator=(Texture&& other) noexcept;               /**< Movable */

    ~Texture();

    const vk::Image& image() const { return m_image; }
    const vk::ImageView& imageView() const { return m_imageView; }
    const vk::Sampler& sampler() const { return m_sampler; }

protected:

    vk::DeviceMemory m_deviceMemory{};
    vk::Image m_image{};
    vk::ImageView m_imageView{};
    vk::Sampler m_sampler{};

    void initializeTexels(const TextureCreateInfo& createInfo);

    void pipelineImageBarrier(
        const vk::CommandBuffer& commandBuffer,
        vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
        vk::PipelineStageFlags srcStage, vk::PipelineStageFlags dstStage,
        vk::AccessFlags srcAccess, vk::AccessFlags dstAccess,
        uint32_t layerCount
    );

    //Static members \|/

    static uint32_t selectMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
};

}
