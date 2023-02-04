﻿/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <vulkan/vulkan.hpp>

#include <RealEngine/renderer/VulkanDeletionQueue.hpp>


namespace RE {

/**
 * @brief Specifies parameters for texture creation
*/
struct TextureCreateInfo {
    //Image-related
    vk::ImageType type = vk::ImageType::e2D;
    vk::Format format = vk::Format::eR8G8B8A8Unorm;
    vk::Extent3D extent = vk::Extent3D{0u, 1u, 1u};
    uint32_t layers = 1u;
    vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eSampled;
    vk::ImageLayout initialLayout = vk::ImageLayout::eReadOnlyOptimal;
    //Memory-related
    vk::MemoryPropertyFlagBits memory = vk::MemoryPropertyFlagBits::eDeviceLocal;
    //Raster-related
    std::vector<unsigned char> texels;
};

/**
 * @brief Represents one (or more) images
*/
class Texture {
    friend class VulkanFixture;
    friend class DescriptorSet;
public:

    /**
     * @brief Constructs Texture
     * @param createInfo Specifies parameters of the texture
    */
    Texture(const TextureCreateInfo& createInfo);

    Texture(const Texture&) = delete;
    Texture(Texture&& other) noexcept;

    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&& other) noexcept;

    ~Texture();

    const vk::Image& image() const { return m_image; }

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
        vk::AccessFlags srcAccess, vk::AccessFlags dstAccess
    );

    //Static members \|/

    static inline const vk::PhysicalDevice* s_physicalDevice = nullptr;
    static inline const vk::Device* s_device = nullptr;
    static inline const vk::CommandBuffer* s_commandBuffer = nullptr;
    static inline VulkanDeletionQueue* s_deletionQueue = nullptr;

    static uint32_t selectMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
};

}
