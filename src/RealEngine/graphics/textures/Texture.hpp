/*!
 *  @author    Dubsky Tomas
 */
#pragma once
#include <glm/vec3.hpp>

#include <RealEngine/graphics/commands/CommandBuffer.hpp>
#include <RealEngine/renderer/VulkanObject.hpp>

namespace re {

/**
 * @brief Specifies parameters for texture creation
 */
struct TextureCreateInfo {
    // Memory-related
    vma::AllocationCreateFlags allocFlags = {};
    vma::MemoryUsage memoryUsage          = vma::MemoryUsage::eAutoPreferDevice;

    // Image-related
    vk::ImageCreateFlags flags = {};
    vk::ImageType type         = vk::ImageType::e2D;
    vk::Format format          = vk::Format::eR8G8B8A8Unorm;
    glm::uvec3 extent{};
    uint32_t layers               = 1u;
    vk::ImageUsageFlags usage     = vk::ImageUsageFlagBits::eSampled;
    vk::ImageLayout initialLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    const void* pNext             = nullptr;

    // ImageView-related
    vk::ImageAspectFlags aspects          = vk::ImageAspectFlagBits::eColor;
    vk::ComponentMapping componentMapping = {}; // Identity mapping

    // Sampler-related
    bool hasSampler      = true; // No sampler is created if this is false
    vk::Filter magFilter = vk::Filter::eNearest;
    vk::Filter minFilter = vk::Filter::eNearest;
    vk::SamplerMipmapMode mipmapMode = vk::SamplerMipmapMode::eNearest;

    // Raster-related
    std::span<unsigned char> texels; // Only for 1 layer color images

    // Debug
    [[no_unique_address]] DebugName<> debugName;
};

/**
 * @brief Represents one (or more) images and, optionally, a sampler
 */
class Texture: public VulkanObject {
public:
    /**
     * @brief Constructs a null Texture with no memory or images
     */
    explicit Texture() {}

    /**
     * @brief Constructs a Texture according to the given parameters
     */
    explicit Texture(const TextureCreateInfo& createInfo);

    Texture(const Texture&)            = delete;  /**< Noncopyable */
    Texture& operator=(const Texture&) = delete;  /**< Noncopyable */

    Texture(Texture&& other) noexcept;            /**< Movable */
    Texture& operator=(Texture&& other) noexcept; /**< Movable */

    ~Texture();

    const vk::Image& image() const { return m_image; }
    const vk::ImageView& imageView() const { return m_imageView; }
    const vk::Sampler& sampler() const { return m_sampler; }

protected:
    vma::Allocation m_allocation{};
    vk::Image m_image{};
    vk::ImageView m_imageView{};
    vk::Sampler m_sampler{};

    void initializeTexels(const TextureCreateInfo& createInfo);

    void pipelineImageBarrier(
        const CommandBuffer& cb, vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout, vk::PipelineStageFlags srcStage,
        vk::PipelineStageFlags dstStage, vk::AccessFlags srcAccess,
        vk::AccessFlags dstAccess, uint32_t layerCount
    );
};

} // namespace re
